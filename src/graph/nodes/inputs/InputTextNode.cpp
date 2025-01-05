#include "InputTextNode.h"
#include <graph/slots/TextOutputSlot.h>

InputTextNode::InputTextNode(const BaseStyle& vParentStyle)  //
    : InputNode(vParentStyle) {}

bool InputTextNode::init() {
    bool ret = InputNode::init();
    getDatasRef<BaseNodeDatas>().name = "Input text";
    getDatasRef<BaseNodeDatas>().type = "INPUT_TEXT_NODE";
    createChildSlot<TextOutputSlot>();
    return ret && true;
}
