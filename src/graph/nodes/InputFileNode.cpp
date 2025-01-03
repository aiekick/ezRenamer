#include "InputFileNode.h"
#include <graph/slots/FileOutputSlot.h>

InputFileNode::InputFileNode(const BaseStyle& vParentStyle)  //
    : BaseNode(vParentStyle, BaseNodeDatas("Input file", "INPUT_FILE_NODE")) {}

bool InputFileNode::init() {
    bool ret = BaseNode::init();
    createChildSlot<FileOutputSlot>();
    return ret && true;
}
