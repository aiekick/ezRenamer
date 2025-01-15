#include "ExecNode.h"

ExecNode::ExecNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseNodeDatas("", "", ImGui::GetColorU32(ImVec4(0.2f, 0.5f, 0.2f, 1.0f)))) {}

bool ExecNode::init() {
    return Parent::init();
}

BaseSlotWeak ExecNode::findSlotByTypeAndOptionalName(ez::SlotDir vDir, const std::string& vType, const std::string& vName) {
    BaseSlotWeak ret = Parent::findSlotByTypeAndOptionalName(vDir, vType, vName);
    if (ret.expired()) {
        if (!vType.empty()) {
            if (vDir == ez::SlotDir::INPUT) {
                if (!getInputFlowSlot().expired()) {
                    auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(getInputFlowSlot().lock());
                    const auto& datas = base_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>();
                    if (datas.type == vType) {
                        ret = base_slot_ptr; // we have only one slot, so no need to search for name
                    }
                }
            } else if (vDir == ez::SlotDir::OUTPUT) {
                if (!getOutputFlowSlot().expired()) {
                    auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(getOutputFlowSlot().lock());
                    const auto& datas = base_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>();
                    if (datas.type == vType) {
                        ret = base_slot_ptr;  // we have only one slot, so no need to search for name
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

ez::xml::Nodes ExecNode::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node xml;
    xml.addChilds(Parent::getXmlNodes(vUserDatas));
    auto& node = xml.getChildren().back();
    if (!getInputFlowSlot().expired()) {
        auto& slots_in = node.getOrAddChild("inputs");
        auto slot_ptr = getInputFlowSlot().lock();
        const auto& slot_datas = slot_ptr->getDatas<BaseSlot::BaseSlotDatas>();
        slots_in.addChild("slot")
            .addAttribute("name", slot_datas.name)     //
            .addAttribute("type", slot_datas.type)     //
            .addAttribute("gid", slot_ptr->getUuid())  //
            .addAttribute("lid", "MainInFlow");
    }
    if (!getOutputFlowSlot().expired()) {
        auto& slots_out = node.getOrAddChild("outputs");
        auto slot_ptr = getOutputFlowSlot().lock();
        const auto& slot_datas = slot_ptr->getDatas<BaseSlot::BaseSlotDatas>();
        slots_out.addChild("slot")
            .addAttribute("name", slot_datas.name)     //
            .addAttribute("type", slot_datas.type)     //
            .addAttribute("gid", slot_ptr->getUuid())  //
            .addAttribute("lid", "MainOutFlow");
    }
    m_getXmlModule(node.addChild("module").addAttribute("type", getDatas<BaseNodeDatas>().type));
    return xml.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool ExecNode::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    const auto& strName = vNode.getName();
    if (strName == "node") {
        Parent::setFromXmlNodes(vNode, vParent, vUserDatas);
    } else if (strName == "slot") {
        Parent::setFromXmlNodes(vNode, vParent, vUserDatas);
        const auto& lid = vNode.getAttribute("lid");
        if (lid == "MainInFlow") {
            if (!getInputFlowSlot().expired()) {
                auto slot_ptr = getInputFlowSlot().lock();
                slot_ptr->setUuid(vNode.getAttribute<ez::Uuid>("gid"));
            }
        } else if (lid == "MainOutFlow") {
            if (!getOutputFlowSlot().expired()) {
                auto slot_ptr = getOutputFlowSlot().lock();
                slot_ptr->setUuid(vNode.getAttribute<ez::Uuid>("gid"));
            }
        }
    } else if (strName == "module") {
        if (vNode.getAttribute("type") == getDatas<BaseNodeDatas>().type) {
            m_setXmlModule(vNode, vParent);
        }
    }
    return true;
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

BaseSlotWeak ExecNode::m_findSlotById(nd::PinId vId) {
    BaseSlotWeak ret = Parent::m_findSlotById(vId);
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

void ExecNode::m_getXmlModule(ez::xml::Node& vInOutNode) {
}

void ExecNode::m_setXmlModule(const ez::xml::Node& vNode, const ez::xml::Node& vParent) {
}
