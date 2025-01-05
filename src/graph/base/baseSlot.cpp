#include "baseSlot.h"
#include <ezlibs/ezLog.hpp>

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

        if (ImGui::IsItemHovered()) {
            datas.highLighted = true;
            m_drawSlotText(slotCenter, datas.connected, datas.color, datas.color);
        }
    }
}

bool BaseSlot::isAnInput() {
    return getDatas<BaseSlotDatas>().dir == ez::SlotDir::INPUT;
}

bool BaseSlot::isAnOutput() {
    return getDatas<BaseSlotDatas>().dir == ez::SlotDir::OUTPUT;
}

void BaseSlot::drawDebugInfos() {
    ImGui::Text("--------------------");
    ImGui::Text("Slot %s", getDatas<BaseSlotDatas>().name.c_str());
    ImGui::Text(isAnInput() ? "Input" : "Output");
    //  ImGui::Text("Count connections : %u", (uint32_t)linkedSlots.size());
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

void BaseSlot::m_drawSlotText(const ImVec2& /*vCenter*/, bool /*vConnected*/, ImU32 /*vColor*/, ImU32 /*vInnerColor*/) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    if (draw_list) {
        std::string slotName;

        auto& datas = getDatasRef<BaseSlotDatas>();
        if (isAnInput()) {
            slotName = datas.type;
            size_t len = slotName.length();
            if (len > 0) {
                const char* beg = slotName.c_str();
                ImVec2 txtSize = ImGui::CalcTextSize(beg);
                ImVec2 min = ImVec2(m_pos.x - datas.slotIconSize * 0.5f - txtSize.x, m_pos.y - datas.slotIconSize * 0.5f);
                ImVec2 max = min + ImVec2(txtSize.x, datas.slotIconSize);
                draw_list->AddRectFilled(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
                draw_list->AddText(ImVec2(min.x, m_pos.y - txtSize.y * 0.55f), ImColor(200, 200, 200, 255), beg);
            }
        } else if (isAnOutput()) {
            slotName = datas.type;
            size_t len = slotName.length();
            if (len > 0) {
                const char* beg = slotName.c_str();
                ImVec2 txtSize = ImGui::CalcTextSize(beg);
                ImVec2 min = ImVec2(m_pos.x + datas.slotIconSize * 0.5f, m_pos.y - datas.slotIconSize * 0.5f);
                ImVec2 max = min + ImVec2(txtSize.x, datas.slotIconSize);
                draw_list->AddRectFilled(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
                draw_list->AddText(ImVec2(min.x, m_pos.y - txtSize.y * 0.55f), ImColor(200, 200, 200, 255), beg);
            }
        }
    }
}

void BaseSlot::m_drawBaseSlot(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor) {
    UNUSED(vInnerColor);
    UNUSED(vConnected);

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

//////////////////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

ez::xml::Nodes BaseSlot::getXmlNodes(const std::string& /*vUserDatas*/) {
    ez::xml::Node node;
    /*res += vOffset +
        ez::toStr(
               "<slot index=\"%u\" name=\"%s\" type=\"%s\" place=\"%s\" id=\"%u\"/>\n",
               index,
               name.c_str(),
               slotType.c_str(),
               sGetStringFromBaseSlotPlaceEnum(slotPlace).c_str(),
               (uint32_t)GetSlotID());*/
    return node.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool BaseSlot::setFromXmlNodes(const ez::xml::Node& /*vNode*/, const ez::xml::Node& /*vParent*/, const std::string& /*vUserDatas*/) {
    // const auto& strName = vNode.getName();
    // const auto& strValue = vNode.getContent();
    // const auto& strParentName = vParent.getName();

    /*if (strName == "slot" && strParentName == "node") {
        uint32_t _index = 0U;
        std::string _name;
        std::string _type = "NONE";
        BaseSlot::PlaceEnum _place = BaseSlot::PlaceEnum::NONE;
        uint32_t _m_pinID = 0U;

        for (const tinyxml2::XMLAttribute* attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next()) {
            std::string attName = attr->Name();
            std::string attValue = attr->Value();

            if (attName == "index")
                _index = ez::ivariant(attValue).GetU();
            else if (attName == "name")
                _name = attValue;
            else if (attName == "type")
                _type = attValue;
            else if (attName == "place")
                _place = sGetBaseSlotPlaceEnumFromString(attValue);
            else if (attName == "id")
                _m_pinID = ez::ivariant(attValue).GetU();
        }

        if (index == _index && slotType == _type && slotPlace == _place && !idAlreadySetbyXml) {
            m_pinID = _m_pinID;
            idAlreadySetbyXml = true;

            // pour eviter que des slots aient le meme id qu'un nodePtr
            BaseNode::freeNodeId = ez::maxi<uint32_t>(BaseNode::freeNodeId, (uint32_t)GetSlotID()) + 1U;

            return false;
        }
    }*/

    return true;
}
