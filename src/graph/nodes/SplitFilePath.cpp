#include "SplitFilePath.h"
#include <graph/slots/FileInputSlot.h>
#include <graph/slots/StringOutputSlot.h>

SplitFilePath::SplitFilePath(const BaseStyle& vParentStyle)  //
    : BaseNode(vParentStyle, BaseNodeDatas("File path splitter", "FILE_PATH_SPLITTER_NODE")) {}

bool SplitFilePath::init() {
    bool ret = BaseNode::init();
    createChildSlot<FileInputSlot>();
    createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Path";
    createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Name";
    createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Ext";
    return ret && true;
}
