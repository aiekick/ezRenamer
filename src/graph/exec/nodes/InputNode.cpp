#include "InputNode.h"

InputNode::InputNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool InputNode::init() {
    bool ret = Parent::init();
    ret &= Parent::initOutputFlow(getParentStyle(), m_getThis<Parent>());
    getDatasRef<BaseNodeDatas>().color = ImGui::GetColorU32(ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
    return ret;
}
