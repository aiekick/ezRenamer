#include "baseSlot.h"
#include <graph/base/baseNode.h>
#include <graph/base/baseGraph.h>
#include <ezlibs/ezLog.hpp>

#include <graph/base/baseLink.h>

bool BaseSlot::init() {
    if (ez::Slot::init()) {
        m_pinID = getUuid();
        return true;
    }
    return false;
}

void BaseSlot::setRadius(const float vRadius) {
    getDatasRef<BaseSlotDatas>().radius = vRadius;
}

void BaseSlot::setColor(const ImVec4& vColor) {
    getDatasRef<BaseSlotDatas>().color = ImGui::GetColorU32(vColor);
}

void BaseSlot::setPos(const ImVec2& vPos) {
    m_pos = vPos;
}

bool BaseSlot::draw() {
    ImGui::PushID(this);
    auto& datas = getDatasRef<BaseSlotDatas>();
    if (!datas.hidden) {
        if (isAnInput()) {
            nd::BeginPin(m_pinID, nd::PinKind::Input);
            {
                ImGui::BeginHorizontal(m_pinID.AsPointer());
                nd::PinPivotAlignment(ImVec2(0.0f, 0.5f));
                nd::PinPivotSize(ImVec2(0, 0));
                m_drawSlot();
                if (datas.showWidget) {
                    m_drawInputWidget();
                }
                if (!datas.hideName) {
                    ImGui::TextUnformatted(datas.name.c_str());
                }
                ImGui::Spring(1);
                ImGui::EndHorizontal();
            }
            nd::EndPin();
        } else if (isAnOutput()) {
            nd::BeginPin(m_pinID, nd::PinKind::Output);
            {
                ImGui::BeginHorizontal(m_pinID.AsPointer());
                ImGui::Spring(1);
                if (!datas.hideName) {
                    ImGui::TextUnformatted(datas.name.c_str());
                }
                if (datas.showWidget) {
                    m_drawOutputWidget();
                }
                nd::PinPivotAlignment(ImVec2(0.0f, 0.5f));
                nd::PinPivotSize(ImVec2(0, 0));
                m_drawSlot();
                ImGui::EndHorizontal();
            }
            nd::EndPin();
        }
    }
    ImGui::PopID();
    return false;
}

bool BaseSlot::isConnected() const {
    return !m_links.empty();
}

const BaseLinkWeakCnt& BaseSlot::getLinks() const {
    return m_links;
}

void BaseSlot::m_drawSlot() {
    auto& datas = getDatasRef<BaseSlotDatas>();
    ImGui::Dummy(datas.slotIconSize);

    ImRect slotRect = ImGui::GetCurrentContext()->LastItemData.Rect;
    slotRect.Min += datas.slotPadding;
    slotRect.Max += datas.slotPadding;

    ImVec2 slotCenter = slotRect.GetCenter();
    m_pos = slotCenter;

    nd::PinPivotRect(slotCenter, slotCenter);
    nd::PinRect(slotRect.Min, slotRect.Max);

    datas.highLighted = false;

    if (ImGui::IsRectVisible(datas.slotIconSize)) {
        m_drawBaseSlot(slotCenter, datas.connected, datas.color, datas.color);
    }
}

bool BaseSlot::isAnInput() {
    return getDatas<BaseSlotDatas>().dir == ez::SlotDir::INPUT;
}

bool BaseSlot::isAnOutput() {
    return getDatas<BaseSlotDatas>().dir == ez::SlotDir::OUTPUT;
}

size_t BaseSlot::getMaxConnectionCount() const {
    // we get the possibly overrides user count
    auto count = m_getMaxConnectionCount();
    // but we can accept the user change the logic
    // so an input cant accept more than one connection
    // so we clamp it again
    const auto& datas = getDatas<BaseSlotDatas>();
    if (datas.dir == ez::SlotDir::INPUT) { 
        count = 1U;  // always 1 for an input
    } else if (datas.dir == ez::SlotDir::OUTPUT) {
        count = ez::clamp<size_t>(count, 1U, 1024U);  // 1024 is big enough i guess :)
    }
    return count;
}

//////////////////////////////////////////////////////////////////////////////
////// DRAW DEBUG INFOS //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseSlot::drawDebugInfos() {
    const auto& slotDatas = getDatas<BaseSlot::BaseSlotDatas>();
    ImGui::Text("Slot [%s] : %s (%s)", ((slotDatas.dir == ez::SlotDir::INPUT) ? "IN" : "OUT"), slotDatas.name.c_str(), slotDatas.type.c_str());
    for (const auto& link : m_links) {
        auto link_ptr = link.lock();
        ImGui::Indent();
        if (link_ptr != nullptr) {
            link_ptr->drawDebugInfos();
        } else {
            ImGui::Text("Link : [%s]", "Expired");
        }
        ImGui::Unindent();
    }
}

//////////////////////////////////////////////////////////////
//// PRIVATE /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void BaseSlot::m_drawInputWidget() {
    if (!getParentNode().expired()) {
        assert(!m_This.expired());
        auto ptr = getParentNode().lock();
        if (ptr) {
            // ptr->m_drawInputWidget( m_This);
        }
    }
}

void BaseSlot::m_drawOutputWidget() {
    if (!getParentNode().expired()) {
        assert(!m_This.expired());
        auto ptr = getParentNode().lock();
        if (ptr) {
            // ptr->DrawOutputWidget(vBaseNodeState, m_This);
        }
    }
}

void BaseSlot::m_drawHoveredSlotText(const ImVec2& vCenter, bool /*vConnected*/, ImU32 /*vColor*/, ImU32 /*vInnerColor*/) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    if (draw_list) {
        auto& datas = getDatasRef<BaseSlotDatas>();
        datas.highLighted = true;
        if (isAnInput()) {
            size_t len = datas.hoveredInfos.length();
            if (len > 0) {
                const char* beg = datas.hoveredInfos.c_str();
                ImVec2 txtSize = ImGui::CalcTextSize(beg);
                ImVec2 min = ImVec2(vCenter.x - datas.slotIconSize * 0.5f - txtSize.x, vCenter.y - datas.slotIconSize * 0.5f);
                ImVec2 max = min + ImVec2(txtSize.x, datas.slotIconSize);
                draw_list->AddRectFilled(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
                draw_list->AddText(ImVec2(min.x, vCenter.y - txtSize.y * 0.55f), ImColor(200, 200, 200, 255), beg);
            }
        } else if (isAnOutput()) {
            size_t len = datas.hoveredInfos.length();
            if (len > 0) {
                const char* beg = datas.hoveredInfos.c_str();
                ImVec2 txtSize = ImGui::CalcTextSize(beg);
                ImVec2 min = ImVec2(vCenter.x + datas.slotIconSize * 0.5f, vCenter.y - datas.slotIconSize * 0.5f);
                ImVec2 max = min + ImVec2(txtSize.x, datas.slotIconSize);
                draw_list->AddRectFilled(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
                draw_list->AddText(ImVec2(min.x, vCenter.y - txtSize.y * 0.55f), ImColor(200, 200, 200, 255), beg);
            }
        }
    }
}

void BaseSlot::m_drawBaseSlot(const ImVec2& vCenter, bool /*vConnected*/, ImU32 vColor, ImU32 /*vInnerColor*/) {
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

        /*auto slotOutputMouseLeftPtr = BaseSlot::sSlotGraphOutputMouseLeft.lock();  // blue
        if (slotOutputMouseLeftPtr && slotOutputMouseLeftPtr == m_This.lock()) {
            draw_list->AddNgon(vCenter, slotRadius + 2.0f, ImGui::GetColorU32(BaseSlot::sSlotGraphOutputMouseLeftColor), 24, 2.5f);
        }

        auto slotOutputMouseMiddlePtr = BaseSlot::sSlotGraphOutputMouseMiddle.lock();  // green
        if (slotOutputMouseMiddlePtr && slotOutputMouseMiddlePtr == m_This.lock()) {
            draw_list->AddNgon(vCenter, slotRadius + 2.0f, ImGui::GetColorU32(BaseSlot::sSlotGraphOutputMouseMiddleColor), 24, 2.5f);
        }

        auto slotOutputMouseRightPtr = BaseSlot::sSlotGraphOutputMouseRight.lock();  // red
        if (slotOutputMouseRightPtr && slotOutputMouseRightPtr == m_This.lock()) {
            draw_list->AddNgon(vCenter, slotRadius + 2.0f, ImGui::GetColorU32(BaseSlot::sSlotGraphOutputMouseRightColor), 24, 2.5f);
        }*/
    }
}

size_t BaseSlot::m_getMaxConnectionCount() const {
    const auto& datas = getDatas<BaseSlotDatas>();
    return (datas.dir == ez::SlotDir::INPUT ? 1U : 1024U); // 1024 is big enough i guess :)
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

ez::xml::Nodes BaseSlot::getXmlNodes(const std::string& /*vUserDatas*/) {
    ez::xml::Node xml;
    const auto& slot_datas = getDatas<BaseSlotDatas>();
    xml.addChild("slot").addAttribute("name", slot_datas.name).addAttribute("type", slot_datas.type).addAttribute("id", getUuid());
    return xml.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool BaseSlot::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    const auto& strName = vNode.getName();
    const auto& datas = getDatas<BaseSlotDatas>();
    if ((datas.name == vNode.getAttribute("name")) &&  //
        (datas.type == vNode.getAttribute("type"))) {
        setUuid(vNode.getAttribute<ez::Uuid>("id"));
    }
    return false;
}
