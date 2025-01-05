#include "StringInputSlot.h"
#include <graph/nodeManager.h>

StringInputSlot::StringInputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("String", "STRING_SLOT", ez::SlotDir::INPUT, NodeManager::instance())) {}

bool StringInputSlot::init() {
    auto ret = BaseSlot::init();
    return ret && true;
}
