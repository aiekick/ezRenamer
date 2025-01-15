#pragma once

#include <graph/exec/nodes/GeneratorNode.h>

class StringNode : public GeneratorNode {
    typedef GeneratorNode Parent;

public:
    explicit StringNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(StringNode)
    bool init() override;
};