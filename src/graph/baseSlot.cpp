#include <graph/baseSlot.h>

void BaseSlot::setRadius(const float vRadius) {
    getDatasRef<SlotConfig>().radius = vRadius;
}

void BaseSlot::setColor(const ImVec4& vColor) {    
    getDatasRef<SlotConfig>().color = ImGui::GetColorU32(vColor);
}

void BaseSlot::setPos(const ImVec2& vPos) {
    m_pos = vPos;
}

bool BaseSlot::draw() {
    ImGui::PushID(this);

    ImGui::SetCursorPos(m_pos);
    const ImVec2 pinSize(getDatas<SlotConfig>().radius);

    if (getDatas<SlotConfig>().dir == ez::SlotDir::INPUT) {
        m_drawSlot(ImGui::GetCursorPos());
        ImGui::SameLine();
    }
    ImGui::Text("%s", getDatas<SlotConfig>().name.c_str());

    if (getDatas<SlotConfig>().dir == ez::SlotDir::OUTPUT) {
        ImGui::SameLine();
        m_drawSlot(ImGui::GetCursorPos());
    }

    ImGui::PopID();
    return false;
}

bool BaseSlot::m_drawSlot(const ImVec2& vPos) {
    ImGui::SetCursorPos(vPos);
    const auto& btnSize = ImVec2(ImGui::GetTextLineHeight());
    bool ret = ImGui::InvisibleButton("##btn", btnSize);
    const auto& rc = ImGui::GetCurrentContext()->LastItemData.Rect;
    const auto is_hovered = ImGui::IsItemHovered();
    auto* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(2);
    if (is_hovered) {
        draw_list->AddCircleFilled(rc.GetCenter(), getDatas<SlotConfig>().radius, getDatas<SlotConfig>().hovered_color, 12);
    } else {
        draw_list->AddCircleFilled(rc.GetCenter(), getDatas<SlotConfig>().radius, getDatas<SlotConfig>().color, 12);
    }
    return ret;
}
