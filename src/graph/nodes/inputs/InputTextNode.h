#pragma once

#include <graph/exec/nodes/InputNode.h>


class InputTextNode : public InputNode {
    typedef InputNode Parent;

public:
    explicit InputTextNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(InputTextNode)
    bool init() override;
};