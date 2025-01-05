#include "ToolNode.h"

ToolNode::ToolNode(const BaseStyle& vParentStyle)  //
    : ExecNode(vParentStyle, BaseNodeDatas("", "", ImGui::GetColorU32(ImVec4(0.2f, 0.8f, 0.2f, 1.0f)))) {}

bool ToolNode::init() {
    return ExecNode::init() && initInputFlow(getParentStyle()) && initOutputFlow(getParentStyle());
}

bool ToolNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    getInputFlowSlot().lock()->draw();
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    ImGui::Spring(1, 5.0f);
    getOutputFlowSlot().lock()->draw();
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndHorizontal();
    return false;
}
