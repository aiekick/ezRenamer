#include "FileInputSlot.h"
#include <graph/nodeManager.h>

FileInputSlot::FileInputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("File", "FILE_SLOT", ez::SlotDir::INPUT, NodeManager::instance())) {}

bool FileInputSlot::init() {
    auto ret = BaseSlot::init();
    return ret && true;
}
