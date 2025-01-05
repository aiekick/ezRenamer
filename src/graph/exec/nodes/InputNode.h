#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowOutputSlotAbstract.h>

class InputNode : public ExecNode, public FlowOutputSlotAbstract {
public:
    explicit InputNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(InputNode);
    bool init() override;
    BaseSlotWeak findSlotByType(ez::SlotDir vDir, const std::string& vType) override;

protected:
    bool m_drawHeader() override;
    BaseSlotWeak m_findSlot(nd::PinId vId) override;
};

typedef std::shared_ptr<InputNode> InputNodePtr;
typedef std::weak_ptr<InputNode> InputNodeWeak;
