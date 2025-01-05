#include "ToolNode.h"

ToolNode::ToolNode(const BaseStyle& vParentStyle)  //
    : ExecNode(vParentStyle, BaseNodeDatas("", "", ImGui::GetColorU32(ImVec4(0.2f, 0.5f, 0.2f, 1.0f)))) {}

bool ToolNode::init() {
    return ExecNode::init() && initInputFlow(getParentStyle(), m_getThis<ToolNode>()) && initOutputFlow(getParentStyle(), m_getThis<ToolNode>());
}

BaseSlotWeak ToolNode::findSlotByType(ez::SlotDir vDir, const std::string& vType) {
    BaseSlotWeak ret = ExecNode::findSlotByType(vDir, vType);
    if (ret.expired()) {
        if (!vType.empty()) {
            if (vDir == ez::SlotDir::INPUT) {
                auto base_pin_ptr = std::static_pointer_cast<BaseSlot>(getInputFlowSlot().lock());
                if (base_pin_ptr->getDatas<BaseSlot::BaseSlotDatas>().type == vType) {
                    ret = getInputFlowSlot();
                }
            } else if (vDir == ez::SlotDir::OUTPUT) {
                auto base_pin_ptr = std::static_pointer_cast<BaseSlot>(getOutputFlowSlot().lock());
                if (base_pin_ptr->getDatas<BaseSlot::BaseSlotDatas>().type == vType) {
                    ret = getOutputFlowSlot();
                }
            }
        }
    }
    return ret;
}

bool ToolNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    getInputFlowSlot().lock()->drawSlot();
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    ImGui::Spring(1, 5.0f);
    getOutputFlowSlot().lock()->drawSlot();
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndHorizontal();
    return false;
}

bool ToolNode::m_drawHints() {
    bool ret = ExecNode::m_drawHints();
    auto inPtr = getInputFlowSlot().lock();
    if (nd::GetHoveredPin().Get() == inPtr->getUuid()) {
        inPtr->drawHoveredSlotText();
    }
    auto outPtr = getOutputFlowSlot().lock();
    if (nd::GetHoveredPin().Get() == outPtr->getUuid()) {
        outPtr->drawHoveredSlotText();
    }
    return ret;
}

BaseSlotWeak ToolNode::m_findSlot(nd::PinId vId) {
    BaseSlotWeak ret = ExecNode::m_findSlot(vId);
    if (ret.expired()) {
        if (getInputFlowSlot().lock()->getUuid() == vId.Get()) {
            ret = getInputFlowSlot();
        }
        if (getOutputFlowSlot().lock()->getUuid() == vId.Get()) {
            ret = getOutputFlowSlot();
        }
    }
    return ret;
}
