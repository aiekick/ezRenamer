#pragma once

#include <graph/base/baseDefs.h>
#include <graph/base/baseNode.h>
#include <graph/exec/nodes/abstracts/FlowInputSlotAbstract.h>
#include <graph/exec/nodes/abstracts/FlowOutputSlotAbstract.h>

/*
The ExecNode will have flow slot as Input or Output

WARNING :
    The slots creation must be done by the nnode who derive of it
*/
class ExecNode;
typedef std::shared_ptr<ExecNode> ExecNodePtr;
typedef std::weak_ptr<ExecNode> ExecNodeWeak;

class ExecNode : public BaseNode, public FlowInputSlotAbstract, public FlowOutputSlotAbstract {
public:
    explicit ExecNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(ExecNode);
    bool init() override;
    BaseSlotWeak findSlotByType(ez::SlotDir vDir, const std::string& vType) override;

protected:
    bool m_drawHeader() override;
    bool m_drawHints() override;
    BaseSlotWeak m_findSlot(nd::PinId vId) override;
};

typedef std::shared_ptr<ExecNode> ExecNodePtr;
typedef std::weak_ptr<ExecNode> ExecNodeWeak;
