#include <graph/baseNode.h>
#include <graph/baseSlot.h>
#include <ezlibs/ezLog.hpp>


bool BaseNode::drawWidgets(const uint32_t& vFrame) {
    return false;
}

bool BaseNode::drawNodeWidget(const uint32_t& vFrame) {
    return false;
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
    /*
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
    */
    return false;
}

bool BaseNode::m_drawNodeInputSlots() {
    return false;
}

bool BaseNode::m_drawNodeOutputSlots() {
    return false;
}

bool BaseNode::m_drawBegin() {
    ImGui::PushID(this);
    ImGui::BeginGroup();
    //m_canvas.setExternalChannel(ImGui::GetWindowDrawList()->_Splitter._Current);
    ImGui::BeginVertical("node");
    return true;
}

bool BaseNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
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
                const ImVec4 NodePadding;// = getDatas<BaseNodeDatas>().padding;
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
        /*
        snprintf(
            debugBuffer,
            254,
            "Used(%s)\nCell(%i, %i)",
            (getDatas<BaseNodeDatas>().used ? "true" : "false"),
            getDatas<BaseNodeDatas>().cell.x,
            getDatas<BaseNodeDatas>().cell.y);
        */
        ImVec2 txtSize = ImGui::CalcTextSize(debugBuffer);
        drawList->AddText(m_pos - ImVec2(0, txtSize.y), ImGui::GetColorU32(ImGuiCol_Text), debugBuffer);
    }
}