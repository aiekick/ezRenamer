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
    const std::regex base_regex(m_inputPattern.GetText());
    const auto captures_count = base_regex.mark_count();

    std::vector<BaseSlotWeak> slotsToRemove;
    for (const auto &slot: this->m_getOutputSlots()) {
        auto slot_ptr = std::static_pointer_cast<FlowOutputSlot>(slot.lock());
        if (!slot_ptr->isConnected()) {
            slotsToRemove.push_back(slot_ptr);
        }
    }
    // now remove
    for (const auto &slot: slotsToRemove) {
        m_delSlot(slot);
    }

    while (captures_count > m_getOutputSlots().size()) {
        createChildSlot<StringOutputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().hideName = true;
    }
    // now add a slot at end

}
