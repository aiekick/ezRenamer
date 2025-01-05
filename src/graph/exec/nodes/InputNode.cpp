#include "InputNode.h"

InputNode::InputNode(const BaseStyle& vParentStyle)  //
    : ExecNode(vParentStyle, BaseNodeDatas("", "", ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.8f, 1.0f)))) {}

bool InputNode::init() {
    return ExecNode::init() && initOutputFlow(getParentStyle());
}

bool InputNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    getOutputFlowSlot().lock()->draw();
    ImGui::Spring(1, 5.0f);
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndHorizontal();
    return false;
}
