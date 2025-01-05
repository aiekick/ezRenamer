#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>

class SequenceNode : public ExecNode {
    typedef ExecNode Parent;

public:
    explicit SequenceNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(SequenceNode);
    bool init() override;
};

typedef std::shared_ptr<SequenceNode> SequenceNodePtr;
typedef std::weak_ptr<SequenceNode> SequenceNodeWeak;
