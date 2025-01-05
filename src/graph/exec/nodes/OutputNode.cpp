#include "OutputNode.h"
#include <graph/base/baseSlot.h>

OutputNode::OutputNode(const BaseStyle& vParentStyle)  //
    : ExecNode(vParentStyle) {}

bool OutputNode::init() {
    bool ret = ExecNode::init();
    ret &= ExecNode::initInputFlow(getParentStyle(), m_getThis<ExecNode>());
    getDatasRef<BaseNodeDatas>().color = ImGui::GetColorU32(ImVec4(0.5f, 0.2f, 0.2f, 1.0f));
    return ret;
}
