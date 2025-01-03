#pragma once

#include <graph/base/baseNode.h>

/*
InputTextNode

this node give the content of the text pane

it give a ouput slot with a string of the text pane content
*/
class InputTextNode : public BaseNode {
public:
    explicit InputTextNode(const BaseStyle& vParentStyle) : BaseNode(vParentStyle, ez::NodeDatas("Input text", "INPUT_TEXT_NODE")) {}

};