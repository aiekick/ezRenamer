#include "InputNode.h"

InputNode::InputNode(const BaseStyle& vParentStyle)  //
    : ExecNode(vParentStyle) {}

bool InputNode::init() {
    bool ret = ExecNode::init();
    ret &= ExecNode::initOutputFlow(getParentStyle(), m_getThis<InputNode>());
    getDatasRef<BaseNodeDatas>().color = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.5f, 1.0f));
    return ret;
}
