#pragma once

#include <graph/base/baseNode.h>

/*
SplitFilePath

this node will split the File path

input slot : a file path
output slots : 
 - the path
 - the file
 - the extensions
*/
class SplitFilePath : public BaseNode {
public:
    explicit SplitFilePath(const BaseStyle& vParentStyle);

    bool init() override;
};