#include "InputFileNode.h"
#include <graph/slots/FileOutputSlot.h>

InputFileNode::InputFileNode(const BaseStyle& vParentStyle)  //
    : InputNode(vParentStyle) {}

bool InputFileNode::init() {
    bool ret = InputNode::init();
    getDatasRef<BaseNodeDatas>().name = "Input file";
    getDatasRef<BaseNodeDatas>().type = "INPUT_FILE_NODE";
    createChildSlot<FileOutputSlot>();
    return ret && true;
}
