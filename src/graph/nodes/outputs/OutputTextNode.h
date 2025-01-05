#pragma once

#include <graph/exec/nodes/OutputNode.h>

class OutputTextNode : public OutputNode {
private:
public:
    explicit OutputTextNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(OutputTextNode);
    bool init() override;
};