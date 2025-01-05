#include "OutputTextNode.h"
#include <graph/slots/TextInputSlot.h>

OutputTextNode::OutputTextNode(const BaseStyle& vParentStyle)  //
    : OutputNode(vParentStyle) {}

bool OutputTextNode::init() {
    bool ret = OutputNode::init();
    getDatasRef<BaseNodeDatas>().name = "Output text";
    getDatasRef<BaseNodeDatas>().type = "OUTPUT_TEXT_NODE";
    createChildSlot<TextInputSlot>();
    return ret && true;
}


