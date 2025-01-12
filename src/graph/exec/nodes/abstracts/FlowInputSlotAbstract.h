#pragma once

#include <array>
#include <graph/exec/slots/FlowInputSlot.h>

class FlowInputSlotAbstract {
private:
    FlowInputSlotPtr m_flowInputSlotPtr = nullptr;

public:
    bool initInputFlow(const BaseStyle& vParentStyle, const BaseNodeWeak& vParentNode) {
        m_flowInputSlotPtr = BaseSlot::create<FlowInputSlot>(vParentStyle);
        if (m_flowInputSlotPtr != nullptr) {
            m_flowInputSlotPtr->setParentNode(vParentNode);
            m_flowInputSlotPtr->setUuid(m_flowInputSlotPtr->getUuid());
            return true;
        }
        return false;
    }

    FlowInputSlotWeak getInputFlowSlot() { return m_flowInputSlotPtr; }

};
