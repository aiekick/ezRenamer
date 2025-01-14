#pragma once

#include <graph/exec/nodes/ToolNode.h>

class RegexNode : public ToolNode {
    typedef ToolNode Parent;

public:
    explicit RegexNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(RegexNode)
    bool init() override;
};