#pragma once

#include <graph/exec/nodes/ActionNode.h>

/*
FileNameRenamerNode

this node will rename a file name

he get the base file name bu the flow slot like all others nodes

by widget you can preview a result
the preview of all 
the finale renaming is done by the app

*/
class FileNameRenamerNode : public ActionNode {
private:
public:
    explicit FileNameRenamerNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(FileNameRenamerNode);
    bool init() override;
};