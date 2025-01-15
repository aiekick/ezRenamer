#include "FileOutputSlot.h"
#include <graph/manager/nodeManager.h>

FileOutputSlot::FileOutputSlot(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseSlotDatas("File", "FILE", ez::SlotDir::OUTPUT, NodeManager::Instance())) {}

bool FileOutputSlot::init() {
    auto ret = Parent::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A File path";
    return ret;
}
