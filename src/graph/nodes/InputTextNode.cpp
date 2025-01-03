#include "InputTextNode.h"
#include <graph/slots/StringOutputSlot.h>

InputTextNode::InputTextNode(const BaseStyle& vParentStyle)  //
    : BaseNode(vParentStyle, BaseNodeDatas("Input text", "INPUT_TEXT_NODE")) {}

bool InputTextNode::init() {
    bool ret = BaseNode::init();
    createChildSlot<StringOutputSlot>();
    return ret && true;
}
