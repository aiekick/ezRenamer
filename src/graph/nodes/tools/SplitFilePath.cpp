#include "SplitFilePath.h"
#include <graph/slots/FileInputSlot.h>
#include <graph/slots/StringOutputSlot.h>

SplitFilePath::SplitFilePath(const BaseStyle& vParentStyle)  //
    : ToolNode(vParentStyle) {}

bool SplitFilePath::init() {
    bool ret = ToolNode::init();
    getDatasRef<BaseNodeDatas>().name = "Split file path";
    getDatasRef<BaseNodeDatas>().type = "FILE_PATH_SPLITTER_NODE";
    createChildSlot<FileInputSlot>();
    createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Path";
    createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Name";
    createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Ext";
    return ret && true;
}
