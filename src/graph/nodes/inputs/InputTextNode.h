#pragma once

#include <graph/exec/nodes/InputNode.h>

/*
InputTextNode

this node give the content of the text pane

it give a ouput slot with a string of the text pane content
*/
class InputTextNode : public InputNode {
public:
    explicit InputTextNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(InputTextNode)
    bool init() override;
};