#include "ReplaceNode.h"
#include <graph/slots/StringInputSlot.h>
#include <graph/slots/StringOutputSlot.h>

ReplaceNode::ReplaceNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool ReplaceNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().name = "Replace";
    getDatasRef<BaseNodeDatas>().type = "REPLACE_NODE";
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().hideName = true;
    createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().hideName = true;
    return ret;
}
