#include "FileInputSlot.h"
#include <graph/nodeManager.h>

FileInputSlot::FileInputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("File", "FILE", ez::SlotDir::INPUT, NodeManager::instance())) {}

bool FileInputSlot::init() {
    auto ret = BaseSlot::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A File path";
    return ret && true;
}
