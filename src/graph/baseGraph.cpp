#include <graph/baseGraph.h>
#include <ezlibs/ezLog.hpp>
#include <imguipack/3rdparty/imgui_node_editor/imgui_node_editor_internal.h>

bool BaseGraph::drawGraph() {
    nd::SetCurrentEditor(m_pCanvas);

    //DrawNodeGraphStyleMenu();

    nd::Begin("GraphNode");

    /*m_BaseNodeState.itemPushId = 1;
    if (!m_ChildNodes.empty()) {
        for (auto& node : m_ChildNodes) {
            auto nodePtr = node.second;
            if (nodePtr) {
                nodePtr->DrawNode(&m_BaseNodeState);
            }
        }

        DrawLinks(&m_BaseNodeState);

        DoCreateLinkOrNode(&m_BaseNodeState);
        DoDeleteLinkOrNode(&m_BaseNodeState);
        DoShorcutsOnNode(&m_BaseNodeState);
    }

    DoPopups(&m_BaseNodeState);*/

    /*nd::Suspend();
    ImVec2 smp = ImGui::GetMousePos();
    ImVec2 cmp = nd::ScreenToCanvas(smp);
    ImGui::SetTooltip("Screen Mouse Pos : %.1f %.1f\nCanvas Mouse Pos : %.1f %.1f\nCanvas offset : %.1f %.1f / %.1f %.1f", smp.x, smp.y, cmp.x, cmp.y, co1.x, co1.y,
    co2.x, co2.y); nd::Resume();*/

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
        nd::GetStyle() = m_baseStyle.graphStyle;
        nd::EnableShortcuts(true);
    }
}

void BaseGraph::m_unit() {
    nd::SetCurrentEditor(m_pCanvas);
    nd::DestroyEditor(m_pCanvas);
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
