#include "FileOutputSlot.h"
#include <graph/nodeManager.h>

FileOutputSlot::FileOutputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("File", "FILE", ez::SlotDir::OUTPUT, NodeManager::instance())) {}

bool FileOutputSlot::init() {
    auto ret = BaseSlot::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A File path";
    return ret && true;
}
