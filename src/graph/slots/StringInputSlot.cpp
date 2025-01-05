#include "StringInputSlot.h"
#include <graph/manager/nodeManager.h>

StringInputSlot::StringInputSlot(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseSlotDatas("String", "STRING", ez::SlotDir::INPUT, NodeManager::instance())) {}

bool StringInputSlot::init() {
    auto ret = Parent::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A String";
    return ret && true;
}
