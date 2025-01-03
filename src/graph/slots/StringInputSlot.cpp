﻿#include "StringInputSlot.h"

StringInputSlot::StringInputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("String", "STRING", ez::SlotDir::INPUT)) {}

bool StringInputSlot::init() {
    auto ret = BaseSlot::init();
    return ret && true;
}