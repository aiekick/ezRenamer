#pragma once

#include <graph/base/baseSlot.h>

class FlowOutputSlot : public BaseSlot {
    typedef BaseSlot Parent;

public:
    explicit FlowOutputSlot(const BaseStyle& vParentStyle);
    bool init() override;

    // for expose protected member
    void drawSlot();
    void drawHoveredSlotText();

protected:
    void m_drawBaseSlot(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor) override;
    size_t m_getMaxConnectionCount() const final;
};

typedef std::shared_ptr<FlowOutputSlot> FlowOutputSlotPtr;
typedef std::weak_ptr<FlowOutputSlot> FlowOutputSlotWeak;
