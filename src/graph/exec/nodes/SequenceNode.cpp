#include "SequenceNode.h"
#include <graph/base/baseSlot.h>
#include <graph/exec/slots/FlowOutputSlot.h>

SequenceNode::SequenceNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool SequenceNode::init() {
    bool ret = Parent::init();
    ret &= Parent::initInputFlow(getParentStyle(), m_getThis<Parent>());
    getDatasRef<BaseNodeDatas>().name = "Sequence";
    getDatasRef<BaseNodeDatas>().type = "SEQUENCE_NODE";
    getDatasRef<BaseNodeDatas>().color = ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    createChildSlot<FlowOutputSlot>();
    return ret;
}

void SequenceNode::m_slotWasJustConnected(const BaseSlotWeak& vOwnNodeSlot, const BaseSlotWeak& vExternNodeSlot) {
    auto slot_ptr = vOwnNodeSlot.lock();
    if (slot_ptr->getDatas().dir == ez::SlotDir::OUTPUT) {
        m_removeUnusedOutputSlotsAndAddOneAtEnd();
    }
}

void SequenceNode::m_slotWasJustDisConnected(const BaseSlotWeak& vOwnNodeSlot, const BaseSlotWeak& vExternNodeSlot) {
    auto slot_ptr = vOwnNodeSlot.lock();
    if (slot_ptr->getDatas().dir == ez::SlotDir::OUTPUT) {
        m_removeUnusedOutputSlotsAndAddOneAtEnd();
    }
}

void SequenceNode::m_removeUnusedOutputSlotsAndAddOneAtEnd() {
    std::vector<BaseSlotWeak> slotsToRemove;
    for (const auto& slot : this->m_getOutputSlots()) {
        auto slot_ptr = std::static_pointer_cast<FlowOutputSlot>(slot.lock());
        if (!slot_ptr->isConnected()) {
            slotsToRemove.push_back(slot_ptr);
        }
    }
    // now remove
    for (const auto& slot : slotsToRemove) {
        m_delSlot(slot);
    }
    // now add a slot at end
    createChildSlot<FlowOutputSlot>();
}
