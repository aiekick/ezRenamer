#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>

class ActionNode : public ExecNode, public FlowInputSlotAbstract {
public:
    explicit ActionNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(ActionNode);
    bool init() override;
    BaseSlotWeak findSlotByType(ez::SlotDir vDir, const std::string& vType) override;

protected:
    bool m_drawHeader() override;
    BaseSlotWeak m_findSlot(nd::PinId vId) override;
};

typedef std::shared_ptr<ActionNode> ActionNodePtr;
typedef std::weak_ptr<ActionNode> ActionNodeWeak;
