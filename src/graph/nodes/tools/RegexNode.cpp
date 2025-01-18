#include "RegexNode.h"
#include <graph/slots/StringInputSlot.h>
#include <graph/slots/StringOutputSlot.h>
#include <regex>
#include <vector>

#define TEST_REGEX_PATTERN "(.*)//(.*)//(.*)"

RegexNode::RegexNode(const BaseStyle &vParentStyle)  //
        : Parent(vParentStyle) {}

bool RegexNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().name = "Regex";
    getDatasRef<BaseNodeDatas>().type = "REGEX_NODE";
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().hideName = true;
    m_inputPattern.AddText(TEST_REGEX_PATTERN);
    m_updateNode();
    return ret;
}

bool RegexNode::drawWidgets() {
    ImGui::Header("Regex");
    bool change = false;
    if (m_inputPattern.DisplayInputText(0.0f, "Regex pattern", "(.*)")) {
        m_updateNode();
        change = true;
    }
    return change;
}

void RegexNode::m_getXmlModule(ez::xml::Node &vInOutNode) {
    vInOutNode.addChild("pattern").setContent(m_inputPattern.GetText());
}

void RegexNode::m_setXmlModule(const ez::xml::Node &vNode, const ez::xml::Node &vParent) {
    auto node_copy = vNode;
    auto* pattern_node_ptr = node_copy.getChild("pattern");
    if (pattern_node_ptr != nullptr){
        m_inputPattern.SetText(pattern_node_ptr->getContent());
        m_updateNode();
    }
}

void RegexNode::m_updateNode() {

    try {
        uint32_t captures_count{0u};
        const std::regex base_regex(m_inputPattern.GetText());
        captures_count = base_regex.mark_count();

        // first loop : delete only not connected slots
        // second loop : delete also connected slots if more than captures count
        bool second_loop_check = false;
        std::vector<BaseSlotWeak> slotsToRemove;
        while (this->m_getOutputSlots().size() > captures_count) {
            for (const auto &slot : this->m_getOutputSlots()) {
                auto slot_ptr = std::static_pointer_cast<FlowOutputSlot>(slot.lock());
                if (!slot_ptr->isConnected() || second_loop_check) {
                    slotsToRemove.push_back(slot_ptr);
                }
            }
            for (const auto &slot : slotsToRemove) {
                m_removeSlot(slot);
            }
            second_loop_check = true;
            slotsToRemove.clear();
        }

        while (captures_count > m_getOutputSlots().size()) {
            createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().hideName = true;
        }
    } catch (std::exception &ex) {
        LogVarError("Regex error : %s", ex.what());
    } catch (...) {
        LogVarError("Unknow regex error");
    }
}
