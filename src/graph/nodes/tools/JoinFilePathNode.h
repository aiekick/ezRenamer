#pragma once

#include <graph/exec/nodes/ToolNode.h>

class JoinFilePathNode : public ToolNode {
    typedef ToolNode Parent;

public:
    explicit JoinFilePathNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(JoinFilePathNode)
    bool init() override;
};