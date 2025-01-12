#pragma once

#include <graph/exec/slots/FlowOutputSlot.h>

class FlowOutputSlotAbstract {
private:
    FlowOutputSlotPtr m_flowOutputSlotPtr = nullptr;

public:
    bool initOutputFlow(const BaseStyle& vParentStyle, const BaseNodeWeak& vParentNode) {
        m_flowOutputSlotPtr = BaseSlot::create<FlowOutputSlot>(vParentStyle);
        if (m_flowOutputSlotPtr != nullptr) {
            m_flowOutputSlotPtr->setParentNode(vParentNode);
            m_flowOutputSlotPtr->setUuid(m_flowOutputSlotPtr->getUuid());
            return true;
        }
        return false;
    }

    FlowOutputSlotWeak getOutputFlowSlot() { return m_flowOutputSlotPtr; }
};
