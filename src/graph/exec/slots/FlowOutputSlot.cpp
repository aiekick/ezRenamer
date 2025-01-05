#include "FlowOutputSlot.h"
#include <graph/nodeManager.h>

FlowOutputSlot::FlowOutputSlot(const BaseStyle& vParentStyle, const BaseSlotDatas& /*vDatas*/)  //
    : BaseSlot(vParentStyle, BaseSlotDatas("Flow", "FLOW_SLOT", ez::SlotDir::OUTPUT, NodeManager::instance())) {}

bool FlowOutputSlot::init() {
    auto ret = BaseSlot::init();
    getDatasRef<BaseSlotDatas>().color = ImGui::GetColorU32(ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    getDatasRef<BaseSlotDatas>().hideName = true;
    return ret && true;
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

