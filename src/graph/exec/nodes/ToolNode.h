#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>
#include <graph/exec/nodes/abstracts/FlowOutputSlotAbstract.h>

class ToolNode : public ExecNode {
public:
    explicit ToolNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(ToolNode);
    bool init() override;
};

typedef std::shared_ptr<ToolNode> ToolNodePtr;
typedef std::weak_ptr<ToolNode> ToolNodeWeak;
