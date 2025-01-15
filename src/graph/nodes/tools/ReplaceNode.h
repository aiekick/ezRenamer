#pragma once

#include <graph/exec/nodes/ToolNode.h>

class ReplaceNode : public ToolNode {
    typedef ToolNode Parent;

public:
    explicit ReplaceNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(ReplaceNode)
    bool init() override;
};