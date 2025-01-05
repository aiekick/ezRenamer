#pragma once

#include <graph/exec/nodes/ToolNode.h>

class FilePathJointer {
public:

};

/*
JoinFilePath

this node will split the File path

input slot : a file path
output slots : 
 - the path
 - the file
 - the extensions
*/
class JoinFilePath : public ToolNode {
public:
    explicit JoinFilePath(const BaseStyle& vParentStyle);
    ENABLE_CLONE(JoinFilePath)
    bool init() override;
};