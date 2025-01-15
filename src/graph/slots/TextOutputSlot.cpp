#include "TextOutputSlot.h"
#include <graph/manager/nodeManager.h>

TextOutputSlot::TextOutputSlot(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseSlotDatas("Text", "TEXT", ez::SlotDir::OUTPUT, NodeManager::Instance())) {}

bool TextOutputSlot::init() {
    auto ret = Parent::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A Text";
    return ret;
}
