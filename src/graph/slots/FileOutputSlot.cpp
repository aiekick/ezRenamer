#include "FileOutputSlot.h"
#include <graph/nodeManager.h>

FileOutputSlot::FileOutputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("File", "FILE_SLOT", ez::SlotDir::OUTPUT, NodeManager::instance())) {}

bool FileOutputSlot::init() {
    auto ret = BaseSlot::init();
    return ret && true;
}
