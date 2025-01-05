#include "InputTextNode.h"
#include <graph/slots/TextOutputSlot.h>

InputTextNode::InputTextNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool InputTextNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().name = "Input text";
    getDatasRef<BaseNodeDatas>().type = "INPUT_TEXT_NODE";
    createChildSlot<TextOutputSlot>();
    return ret && true;
}
