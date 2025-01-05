#include "TextInputSlot.h"
#include <graph/manager/nodeManager.h>

TextInputSlot::TextInputSlot(const BaseStyle& vParentStyle)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("Text", "TEXT", ez::SlotDir::INPUT, NodeManager::instance())) {}

bool TextInputSlot::init() {
    auto ret = BaseSlot::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A Text";
    return ret && true;
}
