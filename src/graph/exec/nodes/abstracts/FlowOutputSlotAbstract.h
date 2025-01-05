#pragma once

#include <graph/exec/slots/FlowOutputSlot.h>

class FlowOutputSlotAbstract {
private:
    FlowOutputSlotPtr m_flowOutputSlotPtr = nullptr;

public:
    bool initOutputFlow(const BaseStyle& vParentStyle) {
        m_flowOutputSlotPtr = BaseSlot::create<FlowOutputSlot>(vParentStyle, {});
        return (m_flowOutputSlotPtr != nullptr);
    }

    FlowOutputSlotWeak getOutputFlowSlot() { return m_flowOutputSlotPtr; }
};
