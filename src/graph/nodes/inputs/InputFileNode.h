#pragma once

#include <graph/exec/nodes/InputNode.h>

class InputFileNode : public InputNode {
public:
    explicit InputFileNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(InputFileNode)
    bool init() override;
};