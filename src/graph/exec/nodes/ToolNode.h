#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>
#include <graph/exec/nodes/abstracts/FlowOutputSlotAbstract.h>

class ToolNode : public ExecNode, public FlowInputSlotAbstract, public FlowOutputSlotAbstract {
public:
    explicit ToolNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(ToolNode);
    bool init() override;
    BaseSlotWeak findSlotByType(ez::SlotDir vDir, const std::string& vType) override;

protected:
    bool m_drawHeader() override;
    bool m_drawHints() override;
    BaseSlotWeak m_findSlot(nd::PinId vId) override;
};

typedef std::shared_ptr<ToolNode> ToolNodePtr;
typedef std::weak_ptr<ToolNode> ToolNodeWeak;
