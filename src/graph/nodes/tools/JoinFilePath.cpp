#include "JoinFilePath.h"
#include <graph/slots/FileOutputSlot.h>
#include <graph/slots/StringInputSlot.h>

JoinFilePath::JoinFilePath(const BaseStyle& vParentStyle)  //
    : ToolNode(vParentStyle) {}

bool JoinFilePath::init() {
    bool ret = ToolNode::init();
    getDatasRef<BaseNodeDatas>().name = "Join file path";
    getDatasRef<BaseNodeDatas>().type = "FILE_PATH_JOINER_NODE";
    createChildSlot<FileOutputSlot>();
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Path";
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Name";
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Ext";
    return ret && true;
}
