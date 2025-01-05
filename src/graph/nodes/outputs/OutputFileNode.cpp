#include "OutputFileNode.h"
#include <graph/slots/FileInputSlot.h>

OutputFileNode::OutputFileNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool OutputFileNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().name = "Output file";
    getDatasRef<BaseNodeDatas>().type = "OUTPUT_FILE_NODE";
    createChildSlot<FileInputSlot>();
    return ret && true;
}


