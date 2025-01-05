#pragma once

#include <graph/exec/nodes/InputNode.h>

class InputFileNode : public InputNode {
    typedef InputNode Parent;

public:
    explicit InputFileNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(InputFileNode)
    bool init() override;
};