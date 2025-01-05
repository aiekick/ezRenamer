#include "OutputTextNode.h"
#include <graph/slots/TextInputSlot.h>

OutputTextNode::OutputTextNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool OutputTextNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().name = "Output text";
    getDatasRef<BaseNodeDatas>().type = "OUTPUT_TEXT_NODE";
    createChildSlot<TextInputSlot>();
    return ret && true;
}


