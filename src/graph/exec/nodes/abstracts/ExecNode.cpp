#include "ExecNode.h"

ExecNode::ExecNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseNodeDatas("", "", ImGui::GetColorU32(ImVec4(0.2f, 0.5f, 0.2f, 1.0f)))) {}

bool ExecNode::init() {
    return Parent::init();
}

BaseSlotWeak ExecNode::findSlotByType(ez::SlotDir vDir, const std::string& vType) {
    BaseSlotWeak ret = Parent::findSlotByType(vDir, vType);
    if (ret.expired()) {
        if (!vType.empty()) {
            if (vDir == ez::SlotDir::INPUT) {
                if (!getInputFlowSlot().expired()) {
                    auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(getInputFlowSlot().lock());
                    if (base_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().type == vType) {
                        ret = getInputFlowSlot();
                    }
                }
            } else if (vDir == ez::SlotDir::OUTPUT) {
                if (!getOutputFlowSlot().expired()) {
                    auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(getOutputFlowSlot().lock());
                    if (base_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().type == vType) {
                        ret = getOutputFlowSlot();
                    }
                }
            }
        }
    }
    return ret;
}

void ExecNode::drawDebugInfos() {
    Parent::drawDebugInfos();
    ImGui::Indent();
    if (!getInputFlowSlot().expired()) {
        getInputFlowSlot().lock()->drawDebugInfos();
    } else {
        // not created by the derived node for a reason
    }
    if (!getOutputFlowSlot().expired()) {
        getOutputFlowSlot().lock()->drawDebugInfos();
    } else {
        // not created by the derived node for a reason
    }
    ImGui::Unindent();
}

bool ExecNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    if (!getInputFlowSlot().expired()) {
        getInputFlowSlot().lock()->drawSlot();
    }
    ImGui::Spring(1, 5.0f);
    const auto& datas = getDatas<BaseNodeDatas>();
    const bool pushed = ImGui::PushStyleColorWithContrast4(datas.color, ImGuiCol_Text, ImGui::CustomStyle::puContrastedTextColor, ImGui::CustomStyle::puContrastRatio);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    if (pushed) {
        ImGui::PopStyleColor();
    }
    ImGui::Spring(1, 5.0f);
    if (!getOutputFlowSlot().expired()) {
        getOutputFlowSlot().lock()->drawSlot();
    }
    ImGui::EndHorizontal();
    return false;
}

bool ExecNode::m_drawHints() {
    bool ret = Parent::m_drawHints();
    if (!getInputFlowSlot().expired()) {
        auto inPtr = getInputFlowSlot().lock();
        if (nd::GetHoveredPin().Get() == inPtr->getUuid()) {
            inPtr->drawHoveredSlotText();
        }
    }
    if (!getOutputFlowSlot().expired()) {
        auto outPtr = getOutputFlowSlot().lock();
        if (nd::GetHoveredPin().Get() == outPtr->getUuid()) {
            outPtr->drawHoveredSlotText();
        }
    }
    return ret;
}

BaseSlotWeak ExecNode::m_findSlot(nd::PinId vId) {
    BaseSlotWeak ret = Parent::m_findSlot(vId);
    if (ret.expired()) {
        if (!getInputFlowSlot().expired()) {
            if (getInputFlowSlot().lock()->getUuid() == vId.Get()) {
                ret = getInputFlowSlot();
            }
        }
        if (!getOutputFlowSlot().expired()) {
            if (getOutputFlowSlot().lock()->getUuid() == vId.Get()) {
                ret = getOutputFlowSlot();
            }
        }
    }
    return ret;
}

BaseLinkWeakCnt ExecNode::m_getConnectedLinks() {
    BaseLinkWeakCnt ret = Parent::m_getConnectedLinks();
    if (!getInputFlowSlot().expired()) {
        ret.tryMerge(getInputFlowSlot().lock()->getLinks());
    }
    if (!getOutputFlowSlot().expired()) {
        ret.tryMerge(getOutputFlowSlot().lock()->getLinks());
    }
    return ret;
}
