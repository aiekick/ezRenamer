#include "FileInputSlot.h"
#include <graph/manager/nodeManager.h>

FileInputSlot::FileInputSlot(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseSlotDatas("File", "FILE", ez::SlotDir::INPUT, NodeManager::Instance())) {}

bool FileInputSlot::init() {
    auto ret = Parent::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A File path";
    return ret;
}
