#include <graph/baseNode.h>
#include <graph/baseSlot.h>
#include <ezlibs/ezLog.hpp>

bool BaseNode::drawGraph() {
    bool ret = false;
    if (m_canvas.begin("##Canvas", ImGui::GetContentRegionAvail())) {
        m_drawGraphNodes();
        m_canvas.end();
    }
    return ret;
}

bool BaseNode::drawNode() {
    if (m_drawBegin()) {
        m_drawHeader();
        m_drawNodeContent();
        m_drawFooter();
        m_drawEnd();
    }
    m_size = ImGui::GetItemRectSize();
    m_pos = ImGui::GetItemRectMin();
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseClicked(0)) {  // bouton gauche click
        }
    }
    return false;
}

ez::RetCodes BaseNode::connectSlots(ez::SlotWeak vFrom, ez::SlotWeak vTo) {
    return m_connectSlots(vFrom, vTo);
}

bool BaseNode::m_drawNodeHeader() {
    ImGui::Text(" %s", m_nodeTitle.c_str());
    return false;
}

bool BaseNode::m_drawNodeContent() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));
    ImGui::BeginHorizontal("content");
    ImGui::Spring(0, 0);
    ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);
    for (auto& p_slot : m_getInputsRef()) {  // slots
        std::static_pointer_cast<BaseSlot>(p_slot)->draw();
    }
    ImGui::EndVertical();
    ImGui::Spring(1, 5.0f);  // pour que BeginVertical soi poussé au bout
    ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);  // 1.0f pour que l'interieur soit aligné sur la fin
    for (auto& p_slot : m_getOutputsRef()) {              // slots
        std::static_pointer_cast<BaseSlot>(p_slot)->draw();
    }
    ImGui::EndVertical();
    ImGui::EndHorizontal();
    ImGui::PopStyleVar();
    return false;
}

bool BaseNode::m_drawNodeInputSlots() {
    return false;
}

bool BaseNode::m_drawNodeOutputSlots() {
    return false;
}

BaseNodeWeak BaseNode::m_getParentGraph() {
    return std::dynamic_pointer_cast<BaseNode>(getParentNode().lock());
}

BaseNodePtr BaseNode::m_getParentGraphPtr() {
    return m_getParentGraph().lock();
}

bool BaseNode::m_drawGraphNodes() {
    bool ret = false;
    auto* drawListPtr = ImGui::GetWindowDrawList();
    if (drawListPtr != nullptr) {
        drawListPtr->ChannelsSplit(3);
        for (auto& node_ptr : getNodes()) {
            auto base_node_ptr = std::dynamic_pointer_cast<BaseNode>(node_ptr);
            if (base_node_ptr != nullptr) {
                ret |= base_node_ptr->drawNode();
            }
        }
        drawListPtr->ChannelsMerge();
    }
    return ret;
}

bool BaseNode::m_drawBegin() {
    ImGui::PushID(this);
    ImGui::BeginGroup();
    m_canvas.setExternalChannel(ImGui::GetWindowDrawList()->_Splitter._Current);
    ImGui::BeginVertical("node");
    return true;
}

bool BaseNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<NodeConfig>().name.c_str());
    ImGui::Spring(1, 5.0f);
    // ImGui::Dummy(ImVec2(0, 24));
    ImGui::EndHorizontal();
    m_headerRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    return false;
}

bool BaseNode::m_drawFooter() {
    return false;
}

bool BaseNode::m_drawEnd() {
    ImGui::EndVertical();
    ImGui::EndGroup();
    if (ImGui::IsItemVisible()) {
        auto drawList = ImGui::GetWindowDrawList();  // GetNodeBackgroundDrawList(nodeID);
        if (drawList) {
            ImGuiContext& g = *GImGui;
            const auto itemRect = g.LastItemData.Rect;
            drawList->AddRectFilled(itemRect.Min, itemRect.Max, ImGui::GetColorU32(ImVec4(0.2, 0.5, 0.2, 0.8)), 2.0f, ImDrawFlags_RoundCornersAll);
            if (m_headerRect.GetSize().y > 0.0f) {
                const ImVec4 NodePadding = getDatas<NodeConfig>().padding;
                const auto halfBorderWidth = 50.0f;

                auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);

                drawList->AddLine(
                    ImVec2(m_headerRect.Min.x - (NodePadding.x - halfBorderWidth), m_headerRect.Max.y - 0.5f),
                    ImVec2(m_headerRect.Max.x + (NodePadding.z - halfBorderWidth), m_headerRect.Max.y - 0.5f),
                    ImColor(255, 255, 255, 96 * alpha / (3 * 255)),
                    1.0f);
            }

            m_displayInfosOnTopOfTheNode();
        } else {
            LogVarDebugInfo("why drawList is null ?? in BaseNode::DrawEnd");
        }
    }
    ImGui::PopID();
    return false;
}

void BaseNode::m_displayInfosOnTopOfTheNode() {
    auto drawList = ImGui::GetWindowDrawList();  // GetNodeBackgroundDrawList(nodeID);
    if (drawList) {
        char debugBuffer[255] = "\0";
        snprintf(
            debugBuffer,
            254,
            "Used(%s)\nCell(%i, %i)" /*\nPos(%.1f, %.1f)\nSize(%.1f, %.1f)*/,
            (getDatas<NodeConfig>().used ? "true" : "false"),
            getDatas<NodeConfig>().cell.x,
            getDatas<NodeConfig>().cell.y /*, pos.x, pos.y, size.x, size.y*/);
        ImVec2 txtSize = ImGui::CalcTextSize(debugBuffer);
        drawList->AddText(m_pos - ImVec2(0, txtSize.y), ImGui::GetColorU32(ImGuiCol_Text), debugBuffer);
    }
}