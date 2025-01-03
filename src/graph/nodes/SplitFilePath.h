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
    explicit SplitFilePath(const BaseStyle& vParentStyle) : BaseNode(vParentStyle, ez::NodeDatas("File splitter", "FILE_PATH_SPLITTER_NODE")) {}
};