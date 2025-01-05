#pragma once

#include <graph/exec/nodes/InputNode.h>


class InputTextNode : public InputNode {
public:
    explicit InputTextNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(InputTextNode)
    bool init() override;
};