#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>

class OutputNode : public ExecNode, public FlowInputSlotAbstract {
public:
    explicit OutputNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(OutputNode);
    bool init() override;
    BaseSlotWeak findSlotByType(ez::SlotDir vDir, const std::string& vType) override;

protected:
    bool m_drawHeader() override;
    bool m_drawHints() override;
    BaseSlotWeak m_findSlot(nd::PinId vId) override;
};

typedef std::shared_ptr<OutputNode> OutputNodePtr;
typedef std::weak_ptr<OutputNode> OutputNodeWeak;
