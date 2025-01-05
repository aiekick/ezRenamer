#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>

class OutputNode : public ExecNode {
    typedef ExecNode Parent;

public:
    explicit OutputNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(OutputNode);
    bool init() override;
};

typedef std::shared_ptr<OutputNode> OutputNodePtr;
typedef std::weak_ptr<OutputNode> OutputNodeWeak;
