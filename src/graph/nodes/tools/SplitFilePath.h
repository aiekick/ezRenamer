#pragma once

#include <graph/exec/nodes/ToolNode.h>

class FilePathSplitter {
public:

};

/*
SplitFilePath

this node will split the File path

input slot : a file path
output slots : 
 - the path
 - the file
 - the extensions
*/
class SplitFilePath : public ToolNode {
public:
    explicit SplitFilePath(const BaseStyle& vParentStyle);
    ENABLE_CLONE(SplitFilePath)
    bool init() override;
};