#pragma once

#include <graph/exec/nodes/OutputNode.h>

class OutputFileNode : public OutputNode {
private:
public:
    explicit OutputFileNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(OutputFileNode);
    bool init() override;
};