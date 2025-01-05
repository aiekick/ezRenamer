#include "InputNode.h"

InputNode::InputNode(const BaseStyle& vParentStyle)  //
    : ExecNode(vParentStyle, BaseNodeDatas("", "", ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.5f, 1.0f)))) {}

bool InputNode::init() {
    return ExecNode::init() && initOutputFlow(getParentStyle(), m_getThis<InputNode>());
}

BaseSlotWeak InputNode::findSlotByType(ez::SlotDir vDir, const std::string& vType) {
    BaseSlotWeak ret = ExecNode::findSlotByType(vDir, vType);
    if (ret.expired()) {
        if (!vType.empty()) {
            if (vDir == ez::SlotDir::OUTPUT) {
                auto base_pin_ptr = std::static_pointer_cast<BaseSlot>(getOutputFlowSlot().lock());
                if (base_pin_ptr->getDatas<BaseSlot::BaseSlotDatas>().type == vType) {
                    ret = getOutputFlowSlot();
                }
            }
        }
    }
    return ret;
}

bool InputNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    ImGui::Spring(1, 5.0f);
    getOutputFlowSlot().lock()->drawSlot();
    ImGui::EndHorizontal();
    return false;
}

bool InputNode::m_drawHints() {
    bool ret = ExecNode::m_drawHints();
    auto ptr = getOutputFlowSlot().lock();
    if (nd::GetHoveredPin().Get() == ptr->getUuid()) {
        ptr->drawHoveredSlotText();
    }
    return ret;
}

BaseSlotWeak InputNode::m_findSlot(nd::PinId vId) {
    BaseSlotWeak ret = ExecNode::m_findSlot(vId);
    if (ret.expired()) {
        if (getOutputFlowSlot().lock()->getUuid() == vId.Get()) {
            ret = getOutputFlowSlot();
        }
    }
    return ret;
}
