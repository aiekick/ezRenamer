#include "StringNode.h"
#include <graph/slots/FileInputSlot.h>
#include <graph/slots/StringOutputSlot.h>

StringNode::StringNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool StringNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().name = "String";
    getDatasRef<BaseNodeDatas>().type = "STRING_GENERATOR_NODE";
    createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "String";
    return ret;
}
