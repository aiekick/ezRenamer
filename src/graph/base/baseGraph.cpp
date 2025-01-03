#include "baseGraph.h"
#include <ezlibs/ezLog.hpp>
#include <imguipack/3rdparty/imgui_node_editor/imgui_node_editor_internal.h>

#define BACKGROUND_CONTEXT_MENU "BackgroundContextMenu"

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

void BaseGraph::m_init() {
    nd::Config config;
    m_pCanvas = nd::CreateEditor(&config);
    if (m_pCanvas != nullptr) {
        nd::SetCurrentEditor(m_pCanvas);
        nd::GetStyle() = m_parentStyle.style;
        nd::EnableShortcuts(true);
    }
}

void BaseGraph::m_unit() {
    nd::SetCurrentEditor(m_pCanvas);
    nd::DestroyEditor(m_pCanvas);
}

void BaseGraph::m_drawPopups() {
    nd::Suspend();
   if (nd::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup(BACKGROUND_CONTEXT_MENU);
        m_openPopupPosition = nd::ScreenToCanvas(ImGui::GetMousePos());
    }
    m_drawBgContextMenuPopup();

    nd::Resume();
}

void BaseGraph::m_drawBgContextMenuPopup() {
    if (ImGui::BeginPopup(BACKGROUND_CONTEXT_MENU)) {
        if (m_bgRightClickAction != nullptr) {
            m_bgRightClickAction(std::static_pointer_cast<BaseGraph>(m_getThis().lock()));
        }
        ImGui::EndPopup();
    }
}

void BaseGraph::m_drawLinks() {
    for (const auto& link : m_links) {
        link.second->draw();
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
            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, _size_.y * 0.15f);
            ImGui::TextUnformatted(label);
        };

        // new link
        nd::PinId startSlotId = 0, endSlotId = 0;
        if (nd::QueryNewLink(&startSlotId, &endSlotId)) {
            auto start_slot_ptr = m_findSlot(startSlotId).lock();
            auto end_slot_ptr = m_findSlot(endSlotId).lock();
            if (start_slot_ptr != nullptr && end_slot_ptr != nullptr) {
                if (start_slot_ptr == end_slot_ptr) {
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
            if (slot_ptr->getDatas<ez::SlotDatas>().dir == ez::SlotDir::OUTPUT) { 
                showLabel("+ Create Node", ImColor(32, 45, 32, 180));  //-V112
                if (nd::AcceptNewItem()) {
                    EZ_TOOLS_DEBUG_BREAK;
                    // we must find a node with input slot of researched type
                    // and create this node
                }
            }
        }
    }
    nd::EndCreate();
}

void BaseGraph::m_doDeleteLinkOrNode() {
    if (nd::BeginDelete()) {
        nd::LinkId linkId = 0;
        while (nd::QueryDeletedLink(&linkId)) {
            if (nd::AcceptDeletedItem()) {
                m_disconnectLink(m_findLink(linkId));
            }
        }
        nd::NodeId nodeId = 0;
        while (nd::QueryDeletedNode(&nodeId)) {
            if (nd::AcceptDeletedItem()) {
                m_delNode(m_findNode(nodeId).lock());
            }
        }
    }
    nd::EndDelete();
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
    auto base_node_ptr = m_findNode(vNodeId).lock();
    if (base_node_ptr != nullptr) {
        auto new_base_node_ptr = BaseNodePtr{nullptr};
        EZ_TOOLS_DEBUG_BREAK;
        if (new_base_node_ptr != nullptr) {
            nd::SetNodePosition(new_base_node_ptr->m_nodeID, new_base_node_ptr->m_pos + vOffsetPos);
        }
    }
}

ez::xml::Nodes BaseGraph::getXmlNodes(const std::string& /*vUserDatas*/) {
    ez::xml::Node node;
    return node.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool BaseGraph::setFromXmlNodes(const ez::xml::Node& /*vNode*/, const ez::xml::Node& /*vParent*/, const std::string& /*vUserDatas*/) {
    return true;
}

void BaseGraph::setBgRightClickAction(const BasicActionFunctor& vFunctor) {
    m_bgRightClickAction = vFunctor;
}

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
        for (const auto& link : m_links) {
            if (link.second->m_linkId == vId) {
                ret = link.second;
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

bool BaseGraph::m_addLink(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd) {
    bool ret = false;
    const auto startPtr = vStart.lock();
    const auto endPtr = vEnd.lock();
    if (startPtr != nullptr && endPtr != nullptr) {
        auto link_ptr = BaseLink::create(m_parentStyle, vStart, vEnd);
        if (link_ptr != nullptr) {
            if (m_links.find(link_ptr->getUuid()) == m_links.end()) {
                m_links[link_ptr->getUuid()] = link_ptr;
                startPtr->m_link = link_ptr;
                endPtr->m_link = link_ptr;
                ret = true;
            }
        }
    }
    return ret;
}

bool BaseGraph::m_breakLink(const BaseLinkWeak& vLink) {
    bool ret = false;
    const auto link_ptr = vLink.lock();
    if (link_ptr != nullptr) {
        if (m_links.find(link_ptr->getUuid()) != m_links.end()) {
            const auto startPtr = link_ptr->m_in.lock();
            if (startPtr != nullptr) {
                startPtr->m_link.reset();
            }
            const auto endPtr = link_ptr->m_out.lock();
            if (endPtr != nullptr) {
                endPtr->m_link.reset();
            }
            m_links.erase(link_ptr->getUuid());
        }
    }
    return ret;
}

bool BaseGraph::m_breakLink(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
    bool ret = false;
    const auto fromPtr = vFrom.lock();
    const auto toPtr = vTo.lock();
    if (fromPtr != nullptr && toPtr != nullptr) {
        if (fromPtr->m_link.lock() == toPtr->m_link.lock()) {
            m_breakLink(fromPtr->m_link);
        } else {
            // ya un bug
            EZ_TOOLS_DEBUG_BREAK;
        }
    }
    return ret;
}

void BaseGraph::m_delOneSideLinks() {
    std::vector<ez::Uuid> links_to_destroy;
    for (const auto& link : m_links) {
        if (link.second->m_in.expired() ||   // the input side is empty OR
            link.second->m_out.expired()) {  // the output side is empty
            links_to_destroy.push_back(link.first);
        }
    }

    for (const auto& link_uuid : links_to_destroy) {
        m_links.erase(link_uuid);
    }
}

bool BaseGraph::m_connectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
    bool ret = false;
    const auto fromPtr = vFrom.lock();
    const auto toPtr = vTo.lock();
    if (fromPtr != nullptr && toPtr != nullptr) {
        if (ez::Graph::m_connectSlots(vFrom, vTo) == ez::RetCodes::SUCCESS) {
            ret = m_addLink(vFrom, vTo);
        }
    }
    return ret;
}

bool BaseGraph::m_disconnectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo) {
    bool ret = false;
    const auto fromPtr = vFrom.lock();
    const auto toPtr = vTo.lock();
    if (fromPtr != nullptr && toPtr != nullptr) {
        if (ez::Graph::m_disconnectSlots(vFrom, vTo) == ez::RetCodes::SUCCESS) {
            ret = m_breakLink(vFrom, vTo);
        }
    }
    return ret;
}

bool BaseGraph::m_disconnectLink(const BaseLinkWeak& vLink) {
    bool ret = false;
    auto link_ptr = vLink.lock();
    if (ez::Graph::m_disconnectSlots(link_ptr->m_in, link_ptr->m_out) == ez::RetCodes::SUCCESS) {
        ret = m_breakLink(link_ptr->m_in, link_ptr->m_out);
    }
    return ret;
}

bool BaseGraph::m_delNode(const BaseNodeWeak& vNode) {
    bool ret = false;

    // when we delete a node, their slot are deleted
    // when a slot is deleted, its link is reseted
    // so we can juste iterate links who are saved in teh graph
    // and when we see a link, who have a least on extermity empty, we can delete the link

    if (ez::Graph::m_delNode(vNode.lock()) == ez::RetCodes::SUCCESS) {
        m_delOneSideLinks();
        ret = true;
    }

    return ret;
}
