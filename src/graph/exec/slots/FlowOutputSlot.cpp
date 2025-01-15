#include "FlowOutputSlot.h"
#include <graph/manager/nodeManager.h>

FlowOutputSlot::FlowOutputSlot(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle, BaseSlotDatas("Flow", "FLOW", ez::SlotDir::OUTPUT, NodeManager::Instance())) {}

bool FlowOutputSlot::init() {
    auto ret = Parent::init();
    getDatasRef<BaseSlotDatas>().hoveredInfos = "Flow";
    getDatasRef<BaseSlotDatas>().color = ImGui::GetColorU32(ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    getDatasRef<BaseSlotDatas>().hideName = true;
    return ret;
}

void FlowOutputSlot::drawSlot() {
    nd::BeginPin(getPinID(), nd::PinKind::Output);
    Parent::m_drawSlot();
    nd::EndPin();
}

void FlowOutputSlot::drawHoveredSlotText() {
    m_drawHoveredSlotText(m_getPos(), false, 0, 0);
}

void FlowOutputSlot::m_drawBaseSlot(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    if (draw_list) {
        auto& datas = getDatasRef<BaseSlotDatas>();
        const auto slotRadius = datas.radius;

        draw_list->AddNgonFilled(vCenter, slotRadius, vColor, 24);

        if (ImGui::IsItemHovered()) {
            ImVec4 _color = ImGui::ColorConvertU32ToFloat4(vColor);
            _color.w = 0.5f;
            draw_list->AddNgon(vCenter, slotRadius + 2.0f, ImGui::GetColorU32(_color), 24, 2.5f);
        }
    }
}

size_t FlowOutputSlot::m_getMaxConnectionCount() const {
    return 1U;  // a flow not accept many connections
}
