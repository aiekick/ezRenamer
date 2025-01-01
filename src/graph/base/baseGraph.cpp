#include "baseGraph.h"
#include <ezlibs/ezLog.hpp>
#include <imguipack/3rdparty/imgui_node_editor/imgui_node_editor_internal.h>

bool BaseGraph::drawGraph() {
    nd::SetCurrentEditor(m_pCanvas);

    //DrawNodeGraphStyleMenu();
    
    nd::Begin("GraphNode");

    if (!getNodes().empty()) {
        for (auto& p_node : getNodesRef()) {
            std::static_pointer_cast<BaseNode>(p_node)->drawNode();
        }

        m_drawLinks();

        //DoCreateLinkOrNode(&m_BaseNodeState);
        //DoDeleteLinkOrNode(&m_BaseNodeState);
        //DoShorcutsOnNode(&m_BaseNodeState);
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
    if (nd::ShowNodeContextMenu(&m_contextMenuNodeId)) {
        ImGui::OpenPopup("NodeContextMenu");
        m_openPopupPosition = ImGui::GetMousePos();
    } else if (nd::ShowPinContextMenu(&m_contextMenuSlotId)) {
        ImGui::OpenPopup("SlotContextMenu");
        m_openPopupPosition = ImGui::GetMousePos();
    } else if (nd::ShowLinkContextMenu(&m_contextMenuLinkId)) {
        ImGui::OpenPopup("LinkContextMenu");
        m_openPopupPosition = ImGui::GetMousePos();
    } else if (nd::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("CreateNewNode");
        m_openPopupPosition = ImGui::GetMousePos();
    }
    m_drawCheckNodePopup();
    m_drawCheckSlotPopup();
    m_drawCheckLinkPopup();
    m_drawNewNodePopup();

    nd::Resume();
}

void BaseGraph::m_drawCheckNodePopup() {

}

void BaseGraph::m_drawCheckSlotPopup() {

}

void BaseGraph::m_drawCheckLinkPopup() {

}

void BaseGraph::m_drawNewNodePopup() {

}

void BaseGraph::m_drawLinks() {
    for (const auto& link : m_links) {
        auto inPtr = link.second->in.lock();
        auto outPtr = link.second->out.lock();
        if (inPtr && outPtr) {
            //nd::Link(link.first, inPtr->pinID, outPtr->pinID, inPtr->color, link.second->thick);
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
////// COPY / PASTE / DUPLICATE //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseGraph::copySelectedNodes() {
    /*
    auto countSelectecdNodes = ax::NodeEditor::GetSelectedObjectCount();
    m_NodesToCopy.resize(countSelectecdNodes);
    ax::NodeEditor::GetActionContextNodes(m_NodesToCopy.data(), (int)m_NodesToCopy.size());
    SetChanged();

    // calcul du point de centre de tout ces nodes
    // sa servira d'offset avec le point de destinatiion
    m_BaseCopyOffset = ImVec2(0, 0);
    for (const auto& id : m_NodesToCopy) {
        m_BaseCopyOffset += ax::NodeEditor::GetNodePosition(id) * 0.5f;
    }
    */
}

void BaseGraph::pasteNodesAtMousePos() {
    /*
    ax::NodeEditor::Suspend();  // necessaire pour avoir le bon MousePos
    auto newOffset = ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePos()) - m_BaseCopyOffset;
    ax::NodeEditor::Resume();
    DuplicateSelectedNodes(newOffset);
    */
}

void BaseGraph::duplicateSelectedNodes(ImVec2 vOffset) {
    /*
    for (auto& it : m_NodesToCopy) {
        DuplicateNode((uint32_t)it.Get(), vOffset);
        SetChanged();
    }
    m_NodesToCopy.clear();
    */
}

ez::xml::Nodes BaseGraph::getXmlNodes(const std::string& /*vUserDatas*/) {
    ez::xml::Node node;
    return node.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool BaseGraph::setFromXmlNodes(const ez::xml::Node& /*vNode*/, const ez::xml::Node& /*vParent*/, const std::string& /*vUserDatas*/) {
    return true;
}
