#include "baseNode.h"
#include <ezlibs/ezLog.hpp>

bool BaseNode::drawWidgets(const uint32_t& vFrame) {
    return false;
}

bool BaseNode::drawNodeWidget(const uint32_t& vFrame) {
    return false;
}

bool BaseNode::drawNode() {
    bool change = false;
    if (m_drawBegin()) {
        change |= m_drawHeader();
        change |= m_drawNodeContent();
        change |= m_drawFooter();
        change |= m_drawEnd();
    }
    m_size = ImGui::GetItemRectSize();
    m_pos = ImGui::GetItemRectMin();
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        }
    }
    return change;
}

ez::xml::Nodes BaseNode::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node node;
    return node.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool BaseNode::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    return true;
}

bool BaseNode::m_drawNodeContent() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));
    ImGui::BeginHorizontal("content");
    ImGui::Spring(0, 0);
    ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);
    for (auto& p_slot : m_getInputSlotsRef()) {  // slots
        std::static_pointer_cast<BaseSlot>(p_slot)->draw();
    }
    ImGui::EndVertical();
    ImGui::Spring(1, 5.0f);  // pour que BeginVertical soi poussé au bout
    ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);  // 1.0f pour que l'interieur soit aligné sur la fin
    for (auto& p_slot : m_getOutputSlotsRef()) {          // slots
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

bool BaseNode::m_drawBegin() {
    nd::BeginNode(m_nodeID);
    ImGui::PushID(m_nodeID.AsPointer());
    ImGui::BeginVertical("node");
    return true;
}

bool BaseNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    ImGui::Spring(1, 5.0f);
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndHorizontal();
    m_headerRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    return false;
}

bool BaseNode::m_drawFooter() {
    return false;
}

bool BaseNode::m_drawEnd() {
    ImGui::EndVertical();
    nd::EndNode();
    if (ImGui::IsItemVisible()) {
        auto drawList = nd::GetNodeBackgroundDrawList(m_nodeID);
        if (drawList) {
            ImGuiContext& g = *GImGui;
            const auto itemRect = g.LastItemData.Rect;
            if (m_headerRect.GetSize().y > 0.0f) {
                const ImVec4 NodePadding = nd::GetStyle().NodePadding;
                const auto halfBorderWidth = nd::GetStyle().NodeBorderWidth * 0.5f;
                drawList->AddRectFilled(
                    m_headerRect.Min - ImVec2(nd::GetStyle().NodePadding.x - halfBorderWidth, halfBorderWidth),
                    m_headerRect.Max + ImVec2(nd::GetStyle().NodePadding.z - halfBorderWidth, 0),
                    m_nodeHeaderColor,
                    nd::GetStyle().NodeRounding,
                    ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight);
                auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);
                drawList->AddLine(
                    ImVec2(m_headerRect.Min.x - (NodePadding.x - halfBorderWidth), m_headerRect.Max.y - 0.5f),
                    ImVec2(m_headerRect.Max.x + (NodePadding.z - halfBorderWidth), m_headerRect.Max.y - 0.5f),
                    ImColor(255, 255, 255, 96 * alpha / (3 * 255)),
                    1.0f);
            }

            //m_displayInfosOnTopOfTheNode();
        } else {
            EZ_TOOLS_DEBUG_BREAK;
        }
    }
    ImGui::PopID();
    return false;
}

void BaseNode::m_displayInfosOnTopOfTheNode() {
    auto drawList = nd::GetNodeBackgroundDrawList(m_nodeID);
    if (drawList) {
        auto datas = getDatas<BaseNodeDatas>();
        const std::string& debugInfos = ez::str::toStr("Used(%s)\nCell(%i, %i)", datas.layout.used, datas.layout.cell.x, datas.layout.cell.y);
        ImVec2 txtSize = ImGui::CalcTextSize(debugInfos.c_str());
        drawList->AddText(m_pos - ImVec2(0, txtSize.y), ImGui::GetColorU32(ImGuiCol_Text), debugInfos.c_str());
    }
}