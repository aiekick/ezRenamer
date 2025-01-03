#include "FileOutputSlot.h"

FileOutputSlot::FileOutputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("File", "FILE", ez::SlotDir::OUTPUT)) {}

bool FileOutputSlot::init() {
    auto ret = BaseSlot::init();
    return ret && true;
}
