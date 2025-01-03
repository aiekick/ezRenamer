#include "FileNameRenamerNode.h"
#include <graph/slots/FileInputSlot.h>

FileNameRenamerNode::FileNameRenamerNode(const BaseStyle& vParentStyle)  //
    : BaseNode(vParentStyle, BaseNodeDatas("File renamer", "FILE_NAME_RENAMER_NODE")) {}

bool FileNameRenamerNode::init() {
    bool ret = BaseNode::init();
    createChildSlot<FileInputSlot>();
    return ret && true;
}


