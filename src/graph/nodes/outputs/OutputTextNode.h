#pragma once

#include <graph/exec/nodes/OutputNode.h>

class OutputTextNode : public OutputNode {
    typedef OutputNode Parent;

private:
public:
    explicit OutputTextNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(OutputTextNode);
    bool init() override;
};