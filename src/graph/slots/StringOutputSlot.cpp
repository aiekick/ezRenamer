#include "StringOutputSlot.h"

StringOutputSlot::StringOutputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("String", "STRING", ez::SlotDir::OUTPUT)) {}

bool StringOutputSlot::init() {
    auto ret = BaseSlot::init();
    return ret && true;
}
