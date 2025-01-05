#include "ToolNode.h"

ToolNode::ToolNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool ToolNode::init() {
    bool ret = Parent::init();
    ret &= Parent::initInputFlow(getParentStyle(), m_getThis<Parent>());
    ret &= Parent::initOutputFlow(getParentStyle(), m_getThis<Parent>());
    getDatasRef<BaseNodeDatas>().color = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.8f, 1.0f));
    return ret;
}

