#pragma once

#include <graph/base/baseNode.h>

/*
InputFileNode

this node give the files list form the files pane

it give a ouput slot with a string of a file
*/
class InputFileNode : public BaseNode {
public:
    explicit InputFileNode(const BaseStyle& vParentStyle) : BaseNode(vParentStyle, ez::NodeDatas("Input file", "INPUT_FILE_NODE")) {}

};