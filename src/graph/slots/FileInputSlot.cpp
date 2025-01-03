#include "FileInputSlot.h"

FileInputSlot::FileInputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("File", "FILE", ez::SlotDir::INPUT)) {}

bool FileInputSlot::init() {
    auto ret = BaseSlot::init();
    return ret && true;
}
