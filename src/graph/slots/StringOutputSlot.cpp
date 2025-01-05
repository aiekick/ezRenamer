#include "StringOutputSlot.h"
#include <graph/nodeManager.h>

StringOutputSlot::StringOutputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("String", "STRING_SLOT", ez::SlotDir::OUTPUT, NodeManager::instance())) {}

bool StringOutputSlot::init() {
    auto ret = BaseSlot::init();
    return ret && true;
}
