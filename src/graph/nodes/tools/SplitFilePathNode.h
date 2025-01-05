#pragma once

#include <graph/exec/nodes/ToolNode.h>

class SplitFilePathNode : public ToolNode {
public:
    explicit SplitFilePathNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(SplitFilePathNode)
    bool init() override;
};