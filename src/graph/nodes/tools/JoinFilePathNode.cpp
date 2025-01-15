#include "JoinFilePathNode.h"
#include <graph/slots/FileOutputSlot.h>
#include <graph/slots/StringInputSlot.h>

JoinFilePathNode::JoinFilePathNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool JoinFilePathNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().name = "Join file path";
    getDatasRef<BaseNodeDatas>().type = "FILE_PATH_JOINER_NODE";
    createChildSlot<FileOutputSlot>();
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Path";
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Name";
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "Ext";
    return ret;
}
