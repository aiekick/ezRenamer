#include "RegexNode.h"
#include <graph/slots/StringInputSlot.h>
#include <graph/slots/StringOutputSlot.h>

RegexNode::RegexNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool RegexNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().name = "Regex";
    getDatasRef<BaseNodeDatas>().type = "REGEX_NODE";
    createChildSlot<StringInputSlot>().lock()->getDatasRef<BaseSlot::BaseSlotDatas>().name = "String";
    return ret;
}

void RegexNode::m_getXmlModule(ez::xml::Node& vInOutNode) {
    vInOutNode.addChild("pattern").setContent(".*");
    EZ_TOOLS_DEBUG_BREAK;
}

void RegexNode::m_setXmlModule(const ez::xml::Node& vNode, const ez::xml::Node& vParent) {
    EZ_TOOLS_DEBUG_BREAK;
}
