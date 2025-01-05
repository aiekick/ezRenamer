#include "StringOutputSlot.h"
#include <graph/nodeManager.h>

StringOutputSlot::StringOutputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("String", "STRING", ez::SlotDir::OUTPUT, NodeManager::instance())) {}

bool StringOutputSlot::init() {
    auto ret = BaseSlot::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A String";
    return ret && true;
}
