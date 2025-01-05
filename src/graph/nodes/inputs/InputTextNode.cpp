#include "InputTextNode.h"
#include <graph/slots/StringOutputSlot.h>

InputTextNode::InputTextNode(const BaseStyle& vParentStyle)  //
    : InputNode(vParentStyle) {}

bool InputTextNode::init() {
    bool ret = InputNode::init();
    getDatasRef<BaseNodeDatas>().name = "Input text";
    getDatasRef<BaseNodeDatas>().type = "INPUT_TEXT_NODE";
    createChildSlot<StringOutputSlot>();
    return ret && true;
}
