#include "OutputNode.h"
#include <grapher/grapher.h>

OutputNode::OutputNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool OutputNode::init() {
    bool ret = Parent::init();
    ret &= Parent::initInputFlow(getParentStyle(), m_getThis<Parent>());
    getDatasRef<BaseNodeDatas>().color = ImGui::GetColorU32(ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    return ret;
}
