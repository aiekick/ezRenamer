#pragma once

#include <graph/exec/nodes/ToolNode.h>

class JoinFilePathNode : public ToolNode {
public:
    explicit JoinFilePathNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(JoinFilePathNode)
    bool init() override;
};