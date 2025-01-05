#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>

class ActionNode : public ExecNode, public FlowInputSlotAbstract {
public:
    explicit ActionNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(ActionNode);
    bool init() override;

protected:
    bool m_drawHeader() override;
};

typedef std::shared_ptr<ActionNode> ActionNodePtr;
typedef std::weak_ptr<ActionNode> ActionNodeWeak;
