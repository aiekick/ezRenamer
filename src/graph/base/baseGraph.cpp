#include "baseGraph.h"
#include <ezlibs/ezLog.hpp>

#include <graph/base/baseNode.h>
#include <graph/base/baseSlot.h>
#include <graph/base/baseLink.h>

#define BACKGROUND_CONTEXT_MENU "BackgroundContextMenu"

BaseGraph::~BaseGraph() {
    unit();
}

bool BaseGraph::init() {
    if (ez::Graph::init()) {
        nd::Config config;
        // disabling json loading/saving
        config.SettingsFile = nullptr;
        config.LoadNodeSettings = nullptr;
        config.LoadSettings = nullptr;
        config.SaveNodeSettings = nullptr;
        config.SaveSettings = nullptr;
        m_pCanvas = nd::CreateEditor(&config);
        if (m_pCanvas != nullptr) {
            setCurrentEditor();
            nd::GetStyle() = m_parentStyle.style;
            nd::EnableShortcuts(true);
            return true;
        }
    }
    return false;
}

void BaseGraph::unit() {
    setCurrentEditor();
    ez::Graph::unit();
}

void BaseGraph::clear() {
    ez::Graph::clear();
    m_links.clear();
}

void BaseGraph::setCurrentEditor() const {
    nd::SetCurrentEditor(m_pCanvas);
}

bool BaseGraph::drawGraph() {
    bool changed = false;
    setCurrentEditor();
    
    const auto canvas_offset = nd::GetCanvasOffset();
    const auto canvas_scale = nd::GetCanvasScale();

    nd::Begin("GraphNode");

    if (!getNodes().empty()) {
        for (auto& node : getNodesRef()) {
            auto node_ptr = std::static_pointer_cast<BaseNode>(node.lock());
            const auto node_pos = node_ptr->m_pos;
            changed |= node_ptr->drawNode();
            if (node_ptr->m_pos != node_pos) {
                m_graphChanged = true;
            }
        }

        m_drawLinks();
        m_doCreateLinkOrNode();
        m_doDeleteLinkOrNode();
        m_doShorcutsOnNode();
    }

    m_slotNotifier.consume();

    m_drawPopups();

    nd::Suspend();
    // draw command
    nd::Resume();

    nd::End();

    if ((canvas_offset != nd::GetCanvasOffset()) || (canvas_scale != nd::GetCanvasScale())) {
        m_graphChanged = true;
    }

    nd::SetCurrentEditor(nullptr);

    return changed;
}

bool BaseGraph::drawNodeWidget(const uint32_t& vFrame) {
    return false;
}

bool BaseGraph::drawWidgets() {
    return false;
}

void BaseGraph::setGraphChanged(bool vFlag) {
    m_graphChanged = vFlag;
}

bool BaseGraph::isGraphChanged() const {
    return m_graphChanged;
}

void BaseGraph::m_drawPopups() {
    nd::Suspend();
    if (nd::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup(BACKGROUND_CONTEXT_MENU);
        m_openPopupPosition = nd::ScreenToCanvas(ImGui::GetMousePos());
        m_PrepareForCreateNodeFromSlot({});  // no slot, maybe needed for reset what was done for slot
    }
    m_drawBgContextMenuPopup();

    nd::Resume();
}

void BaseGraph::m_drawBgContextMenuPopup() {
    if (ImGui::BeginPopup(BACKGROUND_CONTEXT_MENU)) {
        if (m_BgRightClickActionFunctor != nullptr) {
            m_BgRightClickActionFunctor(m_getThis<BaseGraph>());
        }
        ImGui::EndPopup();
    }
}

void BaseGraph::m_drawLinks() {
    for (const auto& link_ptr : m_links) {
        link_ptr->draw();
    }

    // show flow direction
    const auto& datas = getDatas<BaseGraphDatas>();
    if (datas.showFlow && ImGui::IsKeyPressed(datas.showFlowKey)) {
        for (auto& link_ptr : m_links) {
            if (link_ptr->m_type == datas.flowType) {
                nd::Flow(link_ptr->getUuid());
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
////// GAPH NAVIGATION ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::zoomToContent() const {
    if (m_pCanvas) {
        setCurrentEditor();
        nd::NavigateToContent(true);
    }
}

void BaseGraph::navigateToContent() const {
    if (m_pCanvas) {
        setCurrentEditor();
        nd::NavigateToContent(false);
    }
}

void BaseGraph::zoomToSelection() const {
    if (m_pCanvas) {
        setCurrentEditor();
        nd::NavigateToSelection(true);
    }
}

void BaseGraph::navigateToSelection() const {
    if (m_pCanvas) {
        setCurrentEditor();
        nd::NavigateToSelection(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
////// CANVAS QUERY //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

ImVec2 BaseGraph::getMousePos() const {
    // return ImGui::GetMousePosOnOpeningCurrentPopup();
    return ImGui::GetMousePos();
}

//////////////////////////////////////////////////////////////////////////////
////// CONFIGURATION /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

ez::xml::Nodes BaseGraph::getXmlNodes(const std::string& /*vUserDatas*/) {
    ez::xml::Node xml;
    setCurrentEditor();
    xml.addChild("canvas")                              //
        .addAttribute("offset", nd::GetCanvasOffset())  //
        .addAttribute("scale", nd::GetCanvasScale());
    if (!getNodes().empty()) {
        auto& nodes = xml.addChild("nodes");
        for (const auto& node : getNodes()) {
            auto node_ptr = std::static_pointer_cast<BaseNode>(node.lock());
            if (node_ptr != nullptr) {
                nodes.addChilds(node_ptr->getXmlNodes());
            }
        }
    }
    if (!m_links.empty()) {
        auto& links = xml.addChild("links");
        for (const auto& link_ptr : m_links) {
            if (link_ptr != nullptr) {
                links.addChilds(link_ptr->getXmlNodes());
            }
        }
    }
    return xml.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool BaseGraph::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    const auto& strName = vNode.getName();
    if (strName == "canvas") {
        setCurrentEditor();
        nd::SetCanvasView(                         //
            vNode.getAttribute<ImVec2>("offset"),  //
            vNode.getAttribute<float>("scale"));
        return false;
    } else if (strName == "node") {
        if (m_LoadNodeFromXmlFunctor(m_getThis<BaseGraph>(), vNode, vParent)) {
            RecursParsingConfigChilds(vNode, vUserDatas);
        }
        return false;
    } else if (strName == "links") {
        for (const auto& child : vNode.getChildren()) {
            const auto& slot_in = m_findSlotById(child.getAttribute<ez::Uuid>("in"));
            const auto& slot_out = m_findSlotById(child.getAttribute<ez::Uuid>("out"));
            m_connectSlots(slot_in, slot_out);
        }
        return false;
    }
    return true;
}

void BaseGraph::beforeXmlLoading() {
    m_xmlLoading = true;
}

void BaseGraph::afterXmlLoading() {
    m_xmlLoading = false;
    for (auto& node : getNodesRef()) {
        auto node_ptr = std::static_pointer_cast<BaseNode>(node.lock());
        node_ptr->afterXmlLoading();
    }
    m_slotNotifier.clear();
}

//////////////////////////////////////////////////////////////////////////////
////// ACTION FUNCTORS ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::setLoadNodeFromXmlFunctor(const LoadNodeFromXmlFunctor& vFunctor) {
    m_LoadNodeFromXmlFunctor = vFunctor;
}

void BaseGraph::setBgRightClickActionFunctor(const BgRightClickActionFunctor& vFunctor) {
    m_BgRightClickActionFunctor = vFunctor;
}

void BaseGraph::setPrepareForCreateNodeFromSlotActionFunctor(const PrepareForCreateNodeFromSlotActionFunctor& vFunctor) {
    m_PrepareForCreateNodeFromSlotActionFunctor = vFunctor;
}

//////////////////////////////////////////////////////////////////////////////
////// DRAW DEBUG INFOS //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::drawDebugInfos() {
    ImGui::Text("Graph :");
    ImGui::Indent();
    ImGui::Text("Nodes :");
    for (auto& node : getNodesRef()) {
        std::static_pointer_cast<BaseNode>(node.lock())->drawDebugInfos();
    }
    ImGui::Text("Links :");
    for (auto& link_ptr : m_links) {
        ImGui::Indent();
        link_ptr->drawDebugInfos();
        ImGui::Unindent();
    }
    ImGui::Unindent();
}

//////////////////////////////////////////////////////////////////////////////
////// CREATE / DELETE LINKS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::m_drawLabel(const char* vLabel, ImU32 vColor) {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
    auto _size_ = ImGui::CalcTextSize(vLabel);
    auto padding = ImGui::GetStyle().FramePadding;
    auto spacing = ImGui::GetStyle().ItemSpacing;
    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));
    auto rectMin = ImGui::GetCursorScreenPos() - padding;
    auto rectMax = ImGui::GetCursorScreenPos() + _size_ + padding;
    auto draw_list_ptr = ImGui::GetWindowDrawList();
    draw_list_ptr->AddRectFilled(rectMin, rectMax, vColor, _size_.y * 0.15f);
    ImGui::TextUnformatted(vLabel);
}

void BaseGraph::m_doCreateLinkOrNode() {
    nd::PinId startSlotId = 0;
    static ImVec4 slastSlotColor(1.0f, 1.0f, 1.0f, 1.0f);
    if (nd::BeginCreate(&startSlotId, slastSlotColor, 2.0f)) {
        auto start_slot_ptr = m_findSlotById(startSlotId).lock();
        if (start_slot_ptr != nullptr) {
            slastSlotColor = ImGui::ColorConvertU32ToFloat4(start_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().color);
        }

        // new link
        nd::PinId endSlotId = 0;
        if (nd::QueryNewLink(&startSlotId, &endSlotId)) {
            auto start_slot_ptr = m_findSlotById(startSlotId).lock();
            auto end_slot_ptr = m_findSlotById(endSlotId).lock();
            if (start_slot_ptr != nullptr && end_slot_ptr != nullptr) {
                if (end_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().dir == ez::SlotDir::OUTPUT &&
                    start_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().dir == ez::SlotDir::INPUT) {  // if start and end are inverted
                    std::swap(start_slot_ptr, end_slot_ptr);
                }
                slastSlotColor = ImGui::ColorConvertU32ToFloat4(start_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().color);

                if (start_slot_ptr == end_slot_ptr) {  // same slot
                    m_drawLabel("Same Slot", IM_COL32(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (start_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().dir == end_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().dir) {  // same dir
                    m_drawLabel("Same dir", IM_COL32(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (start_slot_ptr->getParentNode().lock() == end_slot_ptr->getParentNode().lock()) {  // same parent node
                    m_drawLabel("Same node", IM_COL32(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (start_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().type != end_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().type) {  // same dir
                    m_drawLabel("Not Same type", IM_COL32(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } /* else if ((end_slot_ptr->getLinks().size() + 1) > end_slot_ptr->getMaxConnectionCount()) {  // slot can have more connections
                    m_drawLabel("the slot cant accept more connections", IM_COL32(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } */
                else if ((start_slot_ptr->getLinks().size() + 1) > start_slot_ptr->getMaxConnectionCount()) {  // slot can have more connections
                    m_drawLabel("the slot cant accept more connections", IM_COL32(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else {
                    if (!end_slot_ptr->getLinks().empty()) {  // slot can have more connections
                        m_drawLabel("+ Re linking", IM_COL32(32, 45, 32, 180));
                    } else {
                        m_drawLabel("+ Create Link", IM_COL32(32, 45, 32, 180));
                    }
                    if (nd::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                        // link or relink
                        if ((end_slot_ptr->getLinks().size() + 1) > end_slot_ptr->getMaxConnectionCount()) {
                            m_disconnectSlot(end_slot_ptr);
                        }
                        m_connectSlots(start_slot_ptr, end_slot_ptr);
                    }
                }
            }
        }

        // new node
        nd::PinId slotId = 0;
        if (nd::QueryNewNode(&slotId)) {
            auto slot_ptr = m_findSlotById(slotId).lock();
            if (slot_ptr != nullptr) {
                if (slot_ptr->getDatas<ez::SlotDatas>().dir == ez::SlotDir::INPUT) {
                    if ((slot_ptr->getLinks().size() + 1) > slot_ptr->getMaxConnectionCount()) {
                        m_drawLabel("the slot cant accept more connections to new node\nbut accept relinking", IM_COL32(32, 45, 32, 180));
                        nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    } else {
#ifdef _DEBUG
                        m_drawLabel("o Redirect link", IM_COL32(32, 45, 32, 180));  //-V112
#endif
                    }
                } else if (slot_ptr->getDatas<ez::SlotDatas>().dir == ez::SlotDir::OUTPUT) {
                    if ((slot_ptr->getLinks().size() + 1) > slot_ptr->getMaxConnectionCount()) {
                        m_drawLabel("the slot cant accept more connections to new node\nbut accept relinking", IM_COL32(32, 45, 32, 180));
                        nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    } else {
                        m_drawLabel("+ Create Node", IM_COL32(32, 45, 32, 180));  //-V112
                        if (nd::AcceptNewItem()) {
                            m_doCreateNodeFromSlot(slot_ptr);
                        }
                    }
                }
            } else {
#ifdef _DEBUG
                m_drawLabel("/_\\ Slot not found", IM_COL32(32, 45, 32, 180));  //-V112
#endif
            }
        }
    } else {
        slastSlotColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    nd::EndCreate();
}

void BaseGraph::m_doDeleteLinkOrNode() {
    if (nd::BeginDelete()) {
        nd::LinkId linkId = 0;
        while (nd::QueryDeletedLink(&linkId)) {
            auto link = m_findLink(linkId);
            if (nd::AcceptDeletedItem()) {  // will delete the link
                m_disconnectLink(link);
            }
        }
        nd::NodeId nodeId = 0;
        while (nd::QueryDeletedNode(&nodeId)) {
            auto node = m_findNode(nodeId);
            if (nd::AcceptDeletedItem()) {
                m_delNode(node);
            }
        }
    }
    nd::EndDelete();
}

void BaseGraph::m_doCreateNodeFromSlot(const BaseSlotWeak& vSlot) {
    if (m_PrepareForCreateNodeFromSlot(vSlot)) {
        ImGui::OpenPopup(BACKGROUND_CONTEXT_MENU);
        m_openPopupPosition = nd::ScreenToCanvas(ImGui::GetMousePos());
    }
}

bool BaseGraph::m_PrepareForCreateNodeFromSlot(const BaseSlotWeak& vSlot) {
    if (m_PrepareForCreateNodeFromSlotActionFunctor != nullptr) {
        return m_PrepareForCreateNodeFromSlotActionFunctor(m_getThis<BaseGraph>(), vSlot);
    }
    return false;
}
    
//////////////////////////////////////////////////////////////////////////////
////// SHORTCUT //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::m_doShorcutsOnNode() {
    if (nd::BeginShortcut()) {
        if (nd::AcceptCopy()) {
            m_copySelectedNodes();
        }
        if (nd::AcceptPaste()) {
            m_pasteNodesAtMousePos();
        }
        if (nd::AcceptDuplicate()) {
            m_duplicateSelectedNodes(m_nodesCopyOffset);
        }
    }
    nd::EndShortcut();
}

//////////////////////////////////////////////////////////////////////////////
////// COPY / PASTE / DUPLICATE //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::m_copySelectedNodes() {
    auto countSelectecdNodes = nd::GetSelectedObjectCount();
    m_nodesToCopy.resize(countSelectecdNodes);
    nd::GetActionContextNodes(m_nodesToCopy.data(), static_cast<int32_t>(m_nodesToCopy.size()));

    // calcul du point de centre de tout ces nodes
    // sa servira d'offset avec le point de destinatiion
    m_nodesCopyOffset = ImVec2(0, 0);
    for (const auto& id : m_nodesToCopy) {
        m_nodesCopyOffset += nd::GetNodePosition(id) * 0.5f;
    }
}

void BaseGraph::m_pasteNodesAtMousePos() {
    nd::Suspend();  // necessaire pour avoir le bon MousePos
    auto newOffset = nd::ScreenToCanvas(ImGui::GetMousePos()) - m_nodesCopyOffset;
    nd::Resume();
    m_duplicateSelectedNodes(newOffset);
}

void BaseGraph::m_duplicateSelectedNodes(const ImVec2& vOffset) {
    for (auto& it : m_nodesToCopy) {
        m_duplicateNode((uint32_t)it.Get(), vOffset);
    }
    m_nodesToCopy.clear();
}

void BaseGraph::m_duplicateNode(uint32_t vNodeId, const ImVec2& vOffsetPos) {
    auto base_node = m_findNode(vNodeId);
    auto base_node_ptr = base_node.lock();
    if (base_node_ptr != nullptr) {
        auto new_pos = base_node_ptr->m_pos + vOffsetPos;
        auto new_base_node_ptr = cloneChildNode(base_node, new_pos);
    }
}

//////////////////////////////////////////////////////////////////////////////
////// FINDERS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

BaseNodeWeak BaseGraph::m_findNode(nd::NodeId vId) {
    BaseNodeWeak ret;
    if (vId) {
        for (const auto& node : getNodes()) {
            auto base_node_ptr = std::static_pointer_cast<BaseNode>(node.lock());
            if (base_node_ptr->m_nodeID == vId) {
                ret = base_node_ptr;
                break;
            }
        }
    }
    return ret;
}

BaseNodeWeak BaseGraph::m_findNodeByName(const std::string& vName) {
    BaseNodeWeak ret;
    if (!vName.empty()) {
        for (const auto& node : getNodes()) {
            if (node.lock()->getDatas<ez::NodeDatas>().name == vName) {
                ret = std::static_pointer_cast<BaseNode>(node.lock());
                break;
            }
        }
    }
    return ret;
}

BaseLinkWeak BaseGraph::m_findLink(nd::LinkId vId) {
    BaseLinkWeak ret;
    if (vId) {
        for (const auto& link_ptr : m_links) {
            if (link_ptr->m_linkId == vId) {
                ret = link_ptr;
                break;
            }
        }
    }
    return ret;
}

BaseLinkWeak BaseGraph::m_findLinkConnectedToSlot(nd::PinId vId) {
    BaseLinkWeak ret;
    if (vId) {
        for (const auto& link_ptr : m_links) {
            if (vId == link_ptr->getInSlot().lock()->getPinID() || //
                vId == link_ptr->getOutSlot().lock()->getPinID()) {
                ret = link_ptr;
                break;
            }
        }
    }
    return ret;
}

BaseSlotWeak BaseGraph::m_findSlotById(nd::PinId vId) {
    BaseSlotWeak ret;
    if (vId) {
        for (const auto& node : getNodes()) {
            auto base_node_ptr = std::static_pointer_cast<BaseNode>(node.lock());
            ret = base_node_ptr->m_findSlotById(vId);
            if (!ret.expired()) {
                break;
            }
        }
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////////
////// ADD / DEL VISUAL LINKS ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool BaseGraph::m_addVisualLink(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd) {
    bool ret = false;
    const auto startPtr = vStart.lock();
    const auto endPtr = vEnd.lock();
    if (startPtr != nullptr && endPtr != nullptr) {
        auto link_ptr = BaseLink::create(m_parentStyle, vStart, vEnd);
        if (link_ptr != nullptr) {
            if (m_links.tryAdd(link_ptr->getUuid(), link_ptr)) {
                if (startPtr->m_links.tryAdd(link_ptr->getUuid(), link_ptr) && endPtr->m_links.tryAdd(link_ptr->getUuid(), link_ptr)) {
                    ret = true;
                } else {
                    EZ_TOOLS_DEBUG_BREAK;
                    LogVarDebugError("Err : fail to add link in both Slots");
                }
            }
        }
    }
    return ret;
}

bool BaseGraph::m_breakVisualLink(const BaseLinkWeak& vLink) {
    bool ret = false;
    const auto link_ptr = vLink.lock();
    if (link_ptr != nullptr) {
        if (m_links.exist(link_ptr->getUuid())) {
            const auto startPtr = link_ptr->m_in.lock();
            if (startPtr != nullptr) {
                startPtr->m_links.erase(link_ptr->getUuid());
            }
            const auto endPtr = link_ptr->m_out.lock();
            if (endPtr != nullptr) {
                endPtr->m_links.erase(link_ptr->getUuid());
            }
            m_links.erase(link_ptr->getUuid());
            ret = true;
        }
    }
    return ret;
}

bool BaseGraph::m_breakVisualLinkConnectedToSlot(const BaseSlotWeak& vSlot) {
    bool ret = false;
    const auto slot_ptr = vSlot.lock();
    if (slot_ptr != nullptr ) {
        std::vector<BaseLinkWeak> links_to_destroy;
        for (const auto& from_link : slot_ptr->m_links) {
            links_to_destroy.push_back(from_link);
            ret = true;
        }
        for (const auto& link : links_to_destroy) {
            m_breakVisualLink(link);
        }
    }
    return ret;
}

bool BaseGraph::m_breakVisualLink(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
    bool ret = false;
    const auto fromPtr = vFrom.lock();
    const auto toPtr = vTo.lock();
    if (fromPtr != nullptr && toPtr != nullptr) {
        // il faut trouver le lien qui existe entre les deux slot
        // et on doit le trouver dans les deux slots
        // si c'est le cas alors on le break
        // si non ya un souci
        std::vector<BaseLinkWeak> found_links;
        for (const auto& from_link : fromPtr->m_links) {
            auto from_link_ptr = from_link.lock();
            if (from_link_ptr != nullptr) {
                for (const auto& to_link : toPtr->m_links) {
                    if (from_link_ptr == to_link.lock()) {
                        found_links.push_back(to_link);
                    }
                }
            }
        }
        if (found_links.size() == 1U) {
            m_breakVisualLink(found_links.front());
        } else {
            // ya un bug
            EZ_TOOLS_DEBUG_BREAK;
        }
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////////
////// CONNECT / DISCONNED ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool BaseGraph::connectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
    return m_connectSlots(vFrom, vTo);
}

bool BaseGraph::disconnectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
    return m_disconnectSlots(vFrom, vTo);
}

bool BaseGraph::disconnectLink(const BaseLinkWeak& vLink) {
    return m_disconnectLink(vLink);
}

bool BaseGraph::m_connectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
    bool ret = false;
    const auto fromPtr = vFrom.lock();
    const auto toPtr = vTo.lock();
    if (fromPtr != nullptr && toPtr != nullptr) {
        if (ez::Graph::m_connectSlots(vFrom, vTo) == ez::RetCodes::SUCCESS) {
            // first we connect the BaseSlot
            ret = m_addVisualLink(vFrom, vTo);
            // then we notify to the parent BaseNode
            if (!m_xmlLoading) {
                m_slotNotifier.addAction(vFrom, vTo, SlotNotifier::ActionType::CONNECT);
            }
        }
    }
    return ret;
}

bool BaseGraph::m_disconnectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
    bool ret = false;
    const auto fromPtr = vFrom.lock();
    const auto toPtr = vTo.lock();
    if (fromPtr != nullptr && toPtr != nullptr) {
        ez::Graph::m_disconnectSlots(vFrom, vTo);
        // first we disconnect the BaseSlot
        ret = m_breakVisualLink(vFrom, vTo);
        // then we notify to the parent BaseNode
        if (!m_xmlLoading) {
            m_slotNotifier.addAction(vFrom, vTo, SlotNotifier::ActionType::DISCONNECT);
        }
    }
    return ret;
}

bool BaseGraph::m_disconnectSlot(const BaseSlotWeak& vSlot) {
    bool ret = false;
    const auto base_slot_ptr = vSlot.lock();
    if (base_slot_ptr != nullptr) {
        // we will disconnects all
        while (!base_slot_ptr->m_getConnectedSlots().empty()) {
            auto other_slot_ptr = std::static_pointer_cast<BaseSlot>(base_slot_ptr->m_getConnectedSlots().front().lock());
            base_slot_ptr->m_disconnectSlot(other_slot_ptr);
            if (!m_xmlLoading) {
                m_slotNotifier.addAction(base_slot_ptr, other_slot_ptr, SlotNotifier::ActionType::DISCONNECT);
            }
            ret = true;
        }        
        m_breakVisualLinkConnectedToSlot(vSlot);
    }
    return ret;
}


bool BaseGraph::m_disconnectLink(const BaseLinkWeak& vLink) {
    bool ret = false;
    auto link_ptr = vLink.lock();
    if (link_ptr != nullptr) {
        ez::Graph::m_disconnectSlots(link_ptr->m_in, link_ptr->m_out);
        // first we disconnect the BaseLink
        ret = m_breakVisualLink(link_ptr);
        // then we notify to the parent BaseNode
        if (!m_xmlLoading) {
            m_slotNotifier.addAction(link_ptr->m_in, link_ptr->m_out, SlotNotifier::ActionType::DISCONNECT);
        }
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////////
////// SLOT NOTIFIER /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::SlotNotifier::addAction(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd, ActionType vType) {
    Action action;
    action.start = vStart;
    action.end = vEnd;
    action.type = vType;
    m_waitingActions.push_back(action);
}

void BaseGraph::SlotNotifier::consume() {
    for (const auto& action : m_waitingActions) {
        auto start_slot_ptr = action.start.lock();
        auto end_slot_ptr = action.end.lock();
        switch (action.type) {
            case ActionType::CONNECT: {
                if (start_slot_ptr != nullptr) {
                    start_slot_ptr->getParentNode<BaseNode>().lock()->m_slotWasJustConnected(action.start, action.end);
                }
                if (end_slot_ptr != nullptr) {
                    end_slot_ptr->getParentNode<BaseNode>().lock()->m_slotWasJustConnected(action.end, action.start);
                }
            } break;
            case ActionType::DISCONNECT: {
                if (start_slot_ptr != nullptr) {
                    start_slot_ptr->getParentNode<BaseNode>().lock()->m_slotWasJustDisConnected(action.start, action.end);
                }
                if (end_slot_ptr != nullptr) {
                    end_slot_ptr->getParentNode<BaseNode>().lock()->m_slotWasJustDisConnected(action.end, action.start);
                }
            } break;
            default: break;
        }
    }
    clear();
}

void BaseGraph::SlotNotifier::clear() {
    m_waitingActions.clear();
}

//////////////////////////////////////////////////////////////////////////////
////// DEL NODES /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool BaseGraph::m_delNode(const BaseNodeWeak& vNode) {
    auto node_ptr = vNode.lock();
    if (node_ptr != nullptr) {
        // we will frist break all links who are connected to the node
        const auto node_links = node_ptr->m_getConnectedLinks();
        for (const auto& link : node_links) {
            m_disconnectLink(link); // we dont 
        }
    }

    // then we can just delete the node
    return (ez::Graph::m_delNode(vNode.lock()) == ez::RetCodes::SUCCESS);
}
