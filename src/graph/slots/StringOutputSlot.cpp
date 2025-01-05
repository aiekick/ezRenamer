#include "StringOutputSlot.h"
#include <graph/manager/nodeManager.h>

StringOutputSlot::StringOutputSlot(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseSlotDatas("String", "STRING", ez::SlotDir::OUTPUT, NodeManager::instance())) {}

bool StringOutputSlot::init() {
    auto ret = Parent::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A String";
    return ret && true;
}
