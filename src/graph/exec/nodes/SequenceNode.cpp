#include "SequenceNode.h"
#include <graph/base/baseSlot.h>
#include <graph/exec/slots/FlowOutputSlot.h>

SequenceNode::SequenceNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool SequenceNode::init() {
    bool ret = Parent::init();
    ret &= Parent::initInputFlow(getParentStyle(), m_getThis<Parent>());
    getDatasRef<BaseNodeDatas>().name = "Sequence";
    getDatasRef<BaseNodeDatas>().type = "SEQUENCE_NODE";
    getDatasRef<BaseNodeDatas>().color = ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    createChildSlot<FlowOutputSlot>();
    return ret;
}
