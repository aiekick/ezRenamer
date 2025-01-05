#pragma once

#include <graph/exec/nodes/InputNode.h>

/*
InputFileNode

this node give the files list form the files pane

it give a ouput slot with a string of a file
*/
class InputFileNode : public InputNode {
public:
    explicit InputFileNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(InputFileNode)
    bool init() override;
};