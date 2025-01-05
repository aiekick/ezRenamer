#include "ActionNode.h"
#include <graph/base/baseSlot.h>

ActionNode::ActionNode(const BaseStyle& vParentStyle)  //
    : ExecNode(vParentStyle, BaseNodeDatas("", "", ImGui::GetColorU32(ImVec4(0.5f, 0.2f, 0.2f, 1.0f)))) {}

bool ActionNode::init() {
    return ExecNode::init() && initInputFlow(getParentStyle(), m_getThis<ActionNode>());
}

BaseSlotWeak ActionNode::findSlotByType(ez::SlotDir vDir, const std::string& vType) {
    BaseSlotWeak ret = ExecNode::findSlotByType(vDir, vType);
    if (ret.expired()) {
        if (!vType.empty()) {
            if (vDir == ez::SlotDir::INPUT) {
                auto base_pin_ptr = std::static_pointer_cast<BaseSlot>(getInputFlowSlot().lock());
                if (base_pin_ptr->getDatas<BaseSlot::BaseSlotDatas>().type == vType) {
                    ret = getInputFlowSlot();
                }
            }
        }
    }
    return ret;
}

bool ActionNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    getInputFlowSlot().lock()->drawSlot();
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    ImGui::Spring(1, 5.0f);
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndHorizontal();
    return false;
}

bool ActionNode::m_drawHints() {
    bool ret = ExecNode::m_drawHints();
    auto ptr = getInputFlowSlot().lock();
    if (nd::GetHoveredPin().Get() == ptr->getUuid()) {
        ptr->drawHoveredSlotText();
    }
    return ret;
}

BaseSlotWeak ActionNode::m_findSlot(nd::PinId vId) {
    BaseSlotWeak ret = ExecNode::m_findSlot(vId);
    if (ret.expired()) {
        if (getInputFlowSlot().lock()->getUuid() == vId.Get()) {
            ret = getInputFlowSlot();
        }
    }
    return ret;
}
