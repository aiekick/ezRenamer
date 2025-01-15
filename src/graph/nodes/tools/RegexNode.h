#pragma once

#include <graph/exec/nodes/ToolNode.h>

class RegexNode : public ToolNode {
    typedef ToolNode Parent;

public:
    explicit RegexNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(RegexNode)
    bool init() override;
    void m_getXmlModule(ez::xml::Node& vInOutNode) override;
    void m_setXmlModule(const ez::xml::Node& vNode, const ez::xml::Node& vParent) override;
};
