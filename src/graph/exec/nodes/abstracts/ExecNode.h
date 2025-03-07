#pragma once

#include <grapher/grapher.h>
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
    typedef BaseNode Parent;

public:
    explicit ExecNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(ExecNode);
    bool init() override;
    BaseSlotWeak findSlotByTypeAndOptionalName(ez::SlotDir vDir, const std::string& vType, const std::string& vName) override;
    void drawDebugInfos() override;
    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas) override;
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

protected:
    bool m_drawHeader() override;
    bool m_drawHints() override;
    BaseSlotWeak m_findSlotById(nd::PinId vId);
    virtual BaseLinkWeakCnt m_getConnectedLinks();
    virtual void m_getXmlModule(ez::xml::Node& vInOutNode);
    virtual void m_setXmlModule(const ez::xml::Node& vNode, const ez::xml::Node& vParent);
};

typedef std::shared_ptr<ExecNode> ExecNodePtr;
typedef std::weak_ptr<ExecNode> ExecNodeWeak;
