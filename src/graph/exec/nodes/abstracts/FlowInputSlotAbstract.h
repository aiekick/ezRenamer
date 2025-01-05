#pragma once

#include <graph/exec/slots/FlowInputSlot.h>

class FlowInputSlotAbstract {
private:
    FlowInputSlotPtr m_flowInputSlotPtr = nullptr;

public:
    bool initInputFlow(const BaseStyle& vParentStyle) {
        m_flowInputSlotPtr = BaseSlot::create<FlowInputSlot>(vParentStyle, {});
        return (m_flowInputSlotPtr != nullptr);
    }

    FlowInputSlotWeak getInputFlowSlot() { return m_flowInputSlotPtr; }

};
