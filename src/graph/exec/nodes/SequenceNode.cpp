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

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool SequenceNode::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    const auto& strName = vNode.getName();
    const auto& strParentName = vParent.getName();
    if (strName == "node") {
        ExecNode::setFromXmlNodes(vNode, vParent, vUserDatas);
    } else if (strName == "slot") {
        ExecNode::setFromXmlNodes(vNode, vParent, vUserDatas);
        if (strParentName == "outputs") {
            const auto& lid = vNode.getAttribute<int32_t>("lid");
            if (lid == this->m_getOutputSlots().size()) {
                auto slot_ptr = createChildSlot<FlowOutputSlot>().lock();
                slot_ptr->setUuid(vNode.getAttribute<ez::Uuid>("gid"));
            }
        }
    }
    return true;
}

void SequenceNode::beforeXmlLoading() {
    ExecNode::beforeXmlLoading();
}

void SequenceNode::afterXmlLoading() {
    ExecNode::afterXmlLoading();
    m_removeUnusedOutputSlotsAndAddOneAtEnd();
}
