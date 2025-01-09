#include "TextInputSlot.h"
#include <graph/manager/nodeManager.h>

TextInputSlot::TextInputSlot(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseSlotDatas("Text", "TEXT", ez::SlotDir::INPUT, NodeManager::Instance())) {}

bool TextInputSlot::init() {
    auto ret = Parent::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "A Text";
    return ret && true;
}
