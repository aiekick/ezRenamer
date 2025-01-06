#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>

class SequenceNode : public ExecNode {
    friend class ExecManager;
    typedef ExecNode Parent;

public:
    explicit SequenceNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(SequenceNode);
    bool init() override;

protected:
    void m_slotWasJustConnected(const BaseSlotWeak& vOwnNodeSlot, const BaseSlotWeak& vExternNodeSlot) final;
    void m_slotWasJustDisConnected(const BaseSlotWeak& vOwnNodeSlot, const BaseSlotWeak& vExternNodeSlot) final;
    // will remove all unused slots
    // and will ensure than there is alwaus one free output slot
    // at end. so all used slots must be before it
    void m_removeUnusedOutputSlotsAndAddOneAtEnd();
};

typedef std::shared_ptr<SequenceNode> SequenceNodePtr;
typedef std::weak_ptr<SequenceNode> SequenceNodeWeak;
