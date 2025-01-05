#pragma once

#include <graph/base/baseSlot.h>

class FlowInputSlot : public BaseSlot {
public:
    explicit FlowInputSlot(const BaseStyle& vParentStyle, const BaseSlotDatas& vDatas);
    bool init() override;

protected:
    void m_drawBaseSlot(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor) override;
};

typedef std::shared_ptr<FlowInputSlot> FlowInputSlotPtr;
typedef std::weak_ptr<FlowInputSlot> FlowInputSlotWeak;
