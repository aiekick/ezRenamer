#include "baseSlot.h"
#include <ezlibs/ezLog.hpp>
#include <graph/nodeManager.h>

BaseSlotPtr BaseSlot::create(const BaseStyle& vParentStyle, const BaseSlotDatas& vSlotDatas) {
    auto slot_ptr = std::make_shared<BaseSlot>(vParentStyle, vSlotDatas);
    slot_ptr->m_setThis(slot_ptr);
    if (!slot_ptr->init()) {
        slot_ptr.reset();
    }
    return slot_ptr;
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

void BaseSlot::notifyConnectionChangeToParent(bool vConnected) {  // va contacter le parent pour lui dire que ce slot est connect� a un autre
    assert(!m_This.expired());
    if (!getParentNode().expired()) {
        auto parentNodePtr = getParentNode().lock();
        if (parentNodePtr) {
            // parentNodePtr->notifyConnectionChangeOfThisSlot(m_This, vConnected);
        }
    }
}

bool BaseSlot::canWeConnectToSlot(BaseSlotWeak vSlot) {
    if (!getParentNode().expired()) {
        auto parentNodePtr = getParentNode().lock();
        if (parentNodePtr) {
            assert(!m_This.expired());
            // return parentNodePtr->canWeConnectSlots(m_This, vSlot);
        }
    }

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
        if (!datas.colorIsSet) {
            datas.color = ImGui::GetColorU32(NodeManager::instance()->getSlotColor(datas.type));
            datas.colorIsSet = true;
        }

        auto u_color = ImGui::GetColorU32(datas.color);

        m_drawBaseSlot(slotCenter, datas.connected, u_color, u_color);

        if (ImGui::IsItemHovered()) {
            datas.highLighted = true;
            m_drawSlotText(slotCenter, datas.connected, u_color, u_color);
        }
    }
}

bool BaseSlot::isAnInput() {
    return getDatas<BaseSlotDatas>().dir == ez::SlotDir::INPUT;
}

bool BaseSlot::isAnOutput() {
    return getDatas<BaseSlotDatas>().dir == ez::SlotDir::OUTPUT;
}

void BaseSlot::notify(const rnm::NotifyEvent& vEvent, const BaseSlotWeak& vEmitterSlot, const BaseSlotWeak& /*vReceiverSlot*/) {
    // one notification can be :
    // - from input to output : Front
    // - from output to input : Back
    // Front or Back will not been used in the same way

    if (vEmitterSlot.expired() || vEmitterSlot.lock() == m_This.lock())  // Front or Back
    {
        // we propagate the notification to the connected slots
        /*for (const auto& otherSlot : linkedSlots) {
            auto otherSlotPtr = otherSlot.lock();
            if (otherSlotPtr) {
                otherSlotPtr->Notify(vEvent, m_This, otherSlot);
            }
        }

        // also to the LMR selected output slots

        // Left
        if (vEmitterSlot.lock() == BaseSlot::sSlotGraphOutputMouseLeft.lock()) {
            BaseNode::SelectForGraphOutput_Callback(BaseSlot::sSlotGraphOutputMouseLeft, ImGuiMouseButton_Left);
        }

        // Middle
        if (vEmitterSlot.lock() == BaseSlot::sSlotGraphOutputMouseMiddle.lock()) {
            BaseNode::SelectForGraphOutput_Callback(BaseSlot::sSlotGraphOutputMouseLeft, ImGuiMouseButton_Middle);
        }

        // Right
        if (vEmitterSlot.lock() == BaseSlot::sSlotGraphOutputMouseRight.lock()) {
            BaseNode::SelectForGraphOutput_Callback(BaseSlot::sSlotGraphOutputMouseLeft, ImGuiMouseButton_Right);
        }*/
    } else  // receiving notification from other slots
    {
        // we treat the notification in herited slots
        /* TreatNotification(vEvent, vEmitterSlot, m_This);

        auto parentPtr = parentNode.lock();
        if (parentPtr) {
            // we treat the notification in parent node
            parentPtr->TreatNotification(vEvent, vEmitterSlot, m_This);
        }

        if (IsAnInput())  // Front
        {
            // front propagate some particular global events
            if (vEvent == GraphIsLoaded || vEvent == NewFrameAvailable || vEvent == SomeTasksWasUpdated) {
                parentPtr->PropagateFrontNotification(vEvent);
            }
        } else if (IsAnOutput())  // Back
        {
            // back propagate some particular global events
            if (vEvent == GraphIsLoaded || vEvent == NewFrameAvailable || vEvent == SomeTasksWasUpdated) {
                parentPtr->PropagateBackNotification(vEvent);
            }
        }*/
    }
}

void BaseSlot::onConnectEvent(const BaseSlotWeak& /*vOtherSlot*/) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::OnConnectEvent catched by the slot \"%s\", some class not implement it. maybe its wanted", getDatas<BaseSlotDatas>().name.c_str());
#endif
}

void BaseSlot::onDisConnectEvent(const BaseSlotWeak& /*vOtherSlot*/) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::OnDisConnectEvent catched by the slot \"%s\", some class not implement it. maybe its wanted", getDatas<BaseSlotDatas>().name.c_str());
#endif
}

void BaseSlot::treatNotification(const rnm::NotifyEvent& /*vEvent*/, const BaseSlotWeak& /*vEmitterSlot*/, const BaseSlotWeak& /*vReceiverSlot*/) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::TreatNotification catched by the slot \"%s\", some class not implement it. maybe its wanted", getDatas<BaseSlotDatas>().name.c_str());
#endif
}

void BaseSlot::sendFrontNotification(const rnm::NotifyEvent& /*vEvent*/) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::SendFrontNotification catched by the slot \"%s\", some class not implement it. maybe its wanted", getDatas<BaseSlotDatas>().name.c_str());
#endif
}

void BaseSlot::sendBackNotification(const rnm::NotifyEvent& /*vEvent*/) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::SendBackNotification catched by the slot \"%s\", some class not implement it. maybe its wanted", getDatas<BaseSlotDatas>().name.c_str());
#endif
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
        std::string slotUType;
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
                ImGui::RenderFrame(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
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
                ImGui::RenderFrame(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
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
