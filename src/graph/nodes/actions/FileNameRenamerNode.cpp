#include "FileNameRenamerNode.h"
#include <graph/slots/FileInputSlot.h>

FileNameRenamerNode::FileNameRenamerNode(const BaseStyle& vParentStyle)  //
    : ActionNode(vParentStyle) {}

bool FileNameRenamerNode::init() {
    bool ret = ActionNode::init();
    getDatasRef<BaseNodeDatas>().name = "File renamer";
    getDatasRef<BaseNodeDatas>().type = "FILE_NAME_RENAMER_NODE";
    createChildSlot<FileInputSlot>();
    return ret && true;
}


