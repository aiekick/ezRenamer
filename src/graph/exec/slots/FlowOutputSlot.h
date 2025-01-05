#pragma once

#include <graph/base/baseSlot.h>

class FlowOutputSlot : public BaseSlot {
public:
    explicit FlowOutputSlot(const BaseStyle& vParentStyle, const BaseSlotDatas& vDatas);
    bool init() override;

    void drawSlot();

protected:
    void m_drawBaseSlot(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor) override;
};

typedef std::shared_ptr<FlowOutputSlot> FlowOutputSlotPtr;
typedef std::weak_ptr<FlowOutputSlot> FlowOutputSlotWeak;
