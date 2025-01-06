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
            nd::SetCurrentEditor(m_pCanvas);
            nd::GetStyle() = m_parentStyle.style;
            nd::EnableShortcuts(true);
            return true;
        }
    }
    return false;
}

void BaseGraph::unit() {
    nd::SetCurrentEditor(m_pCanvas);
    nd::DestroyEditor(m_pCanvas);
    ez::Graph::unit();
}

bool BaseGraph::drawGraph() {
    nd::SetCurrentEditor(m_pCanvas);

    //DrawNodeGraphStyleMenu();
    
    nd::Begin("GraphNode");

    if (!getNodes().empty()) {
        for (auto& node : getNodesRef()) {
            std::static_pointer_cast<BaseNode>(node.lock())->drawNode();
        }

        m_drawLinks();
        m_doCreateLinkOrNode();
        m_doDeleteLinkOrNode();
        m_doShorcutsOnNode();
    }

    m_drawPopups();

    nd::Suspend();
    // draw command
    nd::Resume();

    nd::End();
    nd::SetCurrentEditor(nullptr);

    //DoGraphActions(&m_BaseNodeState);
    return false;
}

bool BaseGraph::drawNodeWidget(const uint32_t& vFrame) {
    return false;
}

bool BaseGraph::drawWidgets(const uint32_t& vFrame) {
    return false;
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
        if (m_BgRightClickAction != nullptr) {
            m_BgRightClickAction(m_getThis<BaseGraph>());
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
        ax::NodeEditor::SetCurrentEditor(m_pCanvas);
        ax::NodeEditor::NavigateToContent(true);
    }
}

void BaseGraph::navigateToContent() const {
    if (m_pCanvas) {
        ax::NodeEditor::SetCurrentEditor(m_pCanvas);
        ax::NodeEditor::NavigateToContent(false);
    }
}

void BaseGraph::zoomToSelection() const {
    if (m_pCanvas) {
        ax::NodeEditor::SetCurrentEditor(m_pCanvas);
        ax::NodeEditor::NavigateToSelection(true);
    }
}

void BaseGraph::navigateToSelection() const {
    if (m_pCanvas) {
        ax::NodeEditor::SetCurrentEditor(m_pCanvas);
        ax::NodeEditor::NavigateToSelection(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
////// CANVAS QUERY //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

ImVec2 BaseGraph::getMousePos() const {
    // return ImGui::GetMousePosOnOpeningCurrentPopup();
    return ImGui::GetMousePos();
}

ImVec2 BaseGraph::getCanvasOffset() const {
    if (m_pCanvas) {
        ax::NodeEditor::SetCurrentEditor(m_pCanvas);
        return ax::NodeEditor::GetCanvasOffset();
    }

    return ImVec2(0, 0);
}

float BaseGraph::getCanvasScale() const {
    if (m_pCanvas) {
        ax::NodeEditor::SetCurrentEditor(m_pCanvas);
        return ax::NodeEditor::GetCanvasScale();
    }

    return 1.0f;
}

void BaseGraph::setCanvasOffset(const ImVec2& vOffset) {
    if (m_pCanvas) {
        ax::NodeEditor::SetCurrentEditor(m_pCanvas);
        ax::NodeEditor::SetCanvasOffset(vOffset);
    }
}

void BaseGraph::setCanvasScale(const float& vScale) {
    if (m_pCanvas) {
        ax::NodeEditor::SetCurrentEditor(m_pCanvas);
        ax::NodeEditor::SetCanvasScale(vScale);
    }
}

//////////////////////////////////////////////////////////////////////////////
////// CONFIGURATION /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

ez::xml::Nodes BaseGraph::getXmlNodes(const std::string& /*vUserDatas*/) {
    ez::xml::Node node;
    return node.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool BaseGraph::setFromXmlNodes(const ez::xml::Node& /*vNode*/, const ez::xml::Node& /*vParent*/, const std::string& /*vUserDatas*/) {
    return true;
}

//////////////////////////////////////////////////////////////////////////////
////// ACTION FUNCTORS ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::setBgRightClickAction(const BgRightClickActionFunctor& vFunctor) {
    m_BgRightClickAction = vFunctor;
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
        link_ptr->drawDebugInfos();
    }
    ImGui::Unindent();
}

//////////////////////////////////////////////////////////////////////////////
////// CREATE / DELETE LINKS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::m_doCreateLinkOrNode() {
    if (nd::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
        auto showLabel = [](const char* label, const ImColor& color) {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            auto _size_ = ImGui::CalcTextSize(label);
            auto padding = ImGui::GetStyle().FramePadding;
            auto spacing = ImGui::GetStyle().ItemSpacing;
            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));
            auto rectMin = ImGui::GetCursorScreenPos() - padding;
            auto rectMax = ImGui::GetCursorScreenPos() + _size_ + padding;
            auto draw_list_ptr = ImGui::GetWindowDrawList();
            draw_list_ptr->AddRectFilled(rectMin, rectMax, color, _size_.y * 0.15f);
            ImGui::TextUnformatted(label);
        };

        // new link
        nd::PinId startSlotId = 0, endSlotId = 0;
        if (nd::QueryNewLink(&startSlotId, &endSlotId)) {
            auto start_slot_ptr = m_findSlot(startSlotId).lock();
            auto end_slot_ptr = m_findSlot(endSlotId).lock();
            if (start_slot_ptr != nullptr && end_slot_ptr != nullptr) {
                if (end_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().dir == ez::SlotDir::OUTPUT &&
                    start_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().dir == ez::SlotDir::INPUT) {  // if start and end are inverted
                    std::swap(start_slot_ptr, end_slot_ptr);
                }
                if (start_slot_ptr == end_slot_ptr) {  // same slot
                    showLabel("Same Slot", ImColor(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (start_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().dir == end_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().dir) {  // same dir
                    showLabel("Same dir", ImColor(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (start_slot_ptr->getParentNode().lock() == end_slot_ptr->getParentNode().lock()) {  // same parent node
                    showLabel("Same node", ImColor(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (start_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().type != end_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().type) {  // same dir
                    showLabel("Not Same type", ImColor(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if ((end_slot_ptr->getLinks().size()+1) > end_slot_ptr->getMaxConnectionCount()) {  // slot can have more conenctions
                    showLabel("the slot cant accept more connections", ImColor(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if ((start_slot_ptr->getLinks().size() + 1) > start_slot_ptr->getMaxConnectionCount()) {  // slot can have more conenctions
                    showLabel("the slot cant accept more connections", ImColor(32, 45, 32, 180));
                    nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else {
                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));  //-V112
                    if (nd::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                        m_connectSlots(start_slot_ptr, end_slot_ptr);
                    }
                }
            }
        }

        // new node
        nd::PinId slotId = 0;
        if (nd::QueryNewNode(&slotId)) {
            auto slot_ptr = m_findSlot(slotId).lock();
            if (slot_ptr != nullptr) {
                if (slot_ptr->getDatas<ez::SlotDatas>().dir == ez::SlotDir::INPUT) {
                    if ((slot_ptr->getLinks().size() + 1) > slot_ptr->getMaxConnectionCount()) {
                        showLabel("the slot cant accept more connections", ImColor(32, 45, 32, 180));
                        nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    } else {
#ifdef _DEBUG
                        showLabel("o Redirect link", ImColor(32, 45, 32, 180));  //-V112
#endif
                    }
                } else if (slot_ptr->getDatas<ez::SlotDatas>().dir == ez::SlotDir::OUTPUT) {
                    if ((slot_ptr->getLinks().size() + 1) > slot_ptr->getMaxConnectionCount()) {
                        showLabel("the slot cant accept more connections", ImColor(32, 45, 32, 180));
                        nd::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    } else {
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));  //-V112
                        if (nd::AcceptNewItem()) {
                            m_doCreateNodeFromSlot(slot_ptr);
                        }
                    }
                }
            } else {
#ifdef _DEBUG
                showLabel("/_\\ Slot not found", ImColor(32, 45, 32, 180));  //-V112
#endif
            }
        }
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
    auto countSelectecdNodes = ax::NodeEditor::GetSelectedObjectCount();
    m_nodesToCopy.resize(countSelectecdNodes);
    ax::NodeEditor::GetActionContextNodes(m_nodesToCopy.data(), static_cast<int32_t>(m_nodesToCopy.size()));

    // calcul du point de centre de tout ces nodes
    // sa servira d'offset avec le point de destinatiion
    m_nodesCopyOffset = ImVec2(0, 0);
    for (const auto& id : m_nodesToCopy) {
        m_nodesCopyOffset += ax::NodeEditor::GetNodePosition(id) * 0.5f;
    }
}

void BaseGraph::m_pasteNodesAtMousePos() {
    ax::NodeEditor::Suspend();  // necessaire pour avoir le bon MousePos
    auto newOffset = ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePos()) - m_nodesCopyOffset;
    ax::NodeEditor::Resume();
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
            }
        }
    }
    return ret;
}

BaseSlotWeak BaseGraph::m_findSlot(nd::PinId vId) {
    BaseSlotWeak ret;
    if (vId) {
        for (const auto& node : getNodes()) {
            auto base_node_ptr = std::static_pointer_cast<BaseNode>(node.lock());
            ret = base_node_ptr->m_findSlot(vId);
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

bool BaseGraph::m_addLink(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd) {
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

bool BaseGraph::m_breakLink(const BaseLinkWeak& vLink) {
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

bool BaseGraph::m_breakLink(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
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
            m_breakLink(found_links.front());
        } else {
            // ya un bug
            EZ_TOOLS_DEBUG_BREAK;
        }
    }
    return ret;
}

void BaseGraph::m_delOneSideLinks() {
    std::vector<ez::Uuid> links_to_destroy;
    for (const auto& link_ptr : m_links) {
        if (link_ptr->m_in.expired() ||  // the input side is empty OR
            link_ptr->m_out.expired()) {  // the output side is empty
            links_to_destroy.push_back(link_ptr->getUuid());
        }
    }

    for (const auto& link_uuid : links_to_destroy) {
        m_links.erase(link_uuid);
    }
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
            ret = m_addLink(vFrom, vTo);
            // then we notify to the parent BaseNode
            fromPtr->getParentNode<BaseNode>().lock()->m_slotWasJustConnected(vFrom, vTo);
            toPtr->getParentNode<BaseNode>().lock()->m_slotWasJustConnected(vTo, vFrom);
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
        ret = m_breakLink(vFrom, vTo);
        // then we notify to the parent BaseNode
        fromPtr->getParentNode<BaseNode>().lock()->m_slotWasJustDisConnected(vFrom, vTo);
        toPtr->getParentNode<BaseNode>().lock()->m_slotWasJustDisConnected(vTo, vFrom);
    }
    return ret;
}

bool BaseGraph::m_disconnectLink(const BaseLinkWeak& vLink) {
    bool ret = false;
    auto link_ptr = vLink.lock();
    if (link_ptr != nullptr) {
        ez::Graph::m_disconnectSlots(link_ptr->m_in, link_ptr->m_out);
        // first we disconnect the BaseLink
        ret = m_breakLink(link_ptr);
        // then we notify to the parent BaseNode
        link_ptr->m_in.lock()->getParentNode<BaseNode>().lock()->m_slotWasJustDisConnected(link_ptr->m_in, link_ptr->m_out);
        link_ptr->m_out.lock()->getParentNode<BaseNode>().lock()->m_slotWasJustDisConnected(link_ptr->m_out, link_ptr->m_in);
    }
    return ret;
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
