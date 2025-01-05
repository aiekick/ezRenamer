#include "StringInputSlot.h"
#include <graph/nodeManager.h>

StringInputSlot::StringInputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("String", "STRING", ez::SlotDir::INPUT, NodeManager::instance())) {}

bool StringInputSlot::init() {
    auto ret = BaseSlot::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A String";
    return ret && true;
}
