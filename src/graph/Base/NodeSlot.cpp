﻿/*
Copyright 2022-2022 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
#include <SoGLSL/Graph/Base/BaseSlot.h>

#include <utility>
#include <SoGLSL/Graph/Base/BaseNode.h>
static const float slotIconSize = 15.0f;

SlotColor::SlotColor()
{
	AddSlotColor("NONE", ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	AddSlotColor("MESH", ImVec4(0.5f, 0.5f, 0.9f, 1.0f));
	AddSlotColor("MESH_GROUP", ImVec4(0.1f, 0.1f, 0.8f, 1.0f));
	AddSlotColor("LIGHT_GROUP", ImVec4(0.9f, 0.9f, 0.1f, 1.0f));
	AddSlotColor("ENVIRONMENT", ImVec4(0.1f, 0.9f, 0.1f, 1.0f));
	AddSlotColor("MERGED", ImVec4(0.1f, 0.5f, 0.9f, 1.0f));
	AddSlotColor("TEXTURE_2D", ImVec4(0.9f, 0.5f, 0.1f, 1.0f));
	AddSlotColor("TEXTURE_2D_GROUP", ImVec4(0.2f, 0.9f, 0.2f, 1.0f));
	AddSlotColor("TEXTURE_3D", ImVec4(0.9f, 0.8f, 0.3f, 1.0f));
	AddSlotColor("MIXED", ImVec4(0.3f, 0.5f, 0.1f, 1.0f));
	AddSlotColor("WIDGET_BOOLEAN", ImVec4(0.8f, 0.7f, 0.6f, 1.0f));
	AddSlotColor("WIDGET_UINT", ImVec4(0.8f, 0.7f, 0.6f, 1.0f));
	AddSlotColor("WIDGET_INT", ImVec4(0.8f, 0.7f, 0.6f, 1.0f));
	AddSlotColor("WIDGET_FLOAT", ImVec4(0.8f, 0.7f, 0.6f, 1.0f));
	AddSlotColor("DEPTH", ImVec4(0.2f, 0.7f, 0.6f, 1.0f));
}

ImVec4 SlotColor::GetSlotColor(const std::string& vBaseSlotType)
{
	ImVec4 res = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);

	if (m_ColorSlots.find(vBaseSlotType) != m_ColorSlots.end())
	{
		res = m_ColorSlots.at(vBaseSlotType);
	}

	return res;
}

void SlotColor::AddSlotColor(const std::string& vBaseSlotType, const ImVec4& vSlotColor)
{
	m_ColorSlots[vBaseSlotType] = vSlotColor;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// STATIC //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

BaseSlotWeak BaseSlot::sSlotGraphOutputMouseLeft;
ImVec4 BaseSlot::sSlotGraphOutputMouseLeftColor;
BaseSlotWeak BaseSlot::sSlotGraphOutputMouseMiddle;
ImVec4 BaseSlot::sSlotGraphOutputMouseMiddleColor;
BaseSlotWeak BaseSlot::sSlotGraphOutputMouseRight;
ImVec4 BaseSlot::sSlotGraphOutputMouseRightColor;

std::string BaseSlot::sGetStringFromBaseSlotPlaceEnum(const BaseSlot::PlaceEnum& vPlaceEnum)
{
	static std::array<std::string, (uint32_t)BaseSlot::PlaceEnum::Count> BaseSlotPlaceString = {
		"NONE",
		"INPUT",
		"OUTPUT",
	};
	if (vPlaceEnum != BaseSlot::PlaceEnum::Count)
		return BaseSlotPlaceString[(int)vPlaceEnum];
	LogVarDebugError("Error, one BaseSlotvBaseSlot::PlaceEnumEnum have no corresponding string, return \"None\"");
	return "NONE";
}

BaseSlot::PlaceEnum BaseSlot::sGetBaseSlotPlaceEnumFromString(const std::string& vBaseSlotPlaceString)
{
	if (vBaseSlotPlaceString == "NONE") return BaseSlot::PlaceEnum::NONE;
	else if (vBaseSlotPlaceString == "INPUT") return BaseSlot::PlaceEnum::INPUT;
	else if (vBaseSlotPlaceString == "OUTPUT") return BaseSlot::PlaceEnum::OUTPUT;
	return BaseSlot::PlaceEnum::NONE;
}

size_t BaseSlot::sGetNewSlotId()
{
	//#define SLOT_ID_OFFSET 100000
	//return SLOT_ID_OFFSET + (++BaseNode::freeNodeId);
	return ++BaseNode::freeNodeId;
}

// static are null when a plugin is loaded
SlotColor* BaseSlot::sGetSlotColors(SlotColor* vCopy, bool vForce)
{
	static SlotColor _SlotColor;
	static SlotColor* _SlotColor_copy = nullptr;
	if (vCopy || vForce)
		_SlotColor_copy = vCopy;
	if (_SlotColor_copy)
		return _SlotColor_copy;
	return &_SlotColor;
}

BaseSlotPtr BaseSlot::Create(BaseSlot vSlot)
{
	auto res = std::make_shared<BaseSlot>(vSlot);
	res->m_This = res;
	return res;
}

std::string BaseSlot::sGetSlotNameFromStageAndName(const std::string& vStage, const std::string& vName) {
    if (!vStage.empty() && !vName.empty()) {
        return ez::toStr("%s_%s", vStage.c_str(), vName.c_str());
	}
    return {};
}

std::pair<std::string, std::string> BaseSlot::sGetStageAndNameFromSlotName(const std::string& vSlotName) {
    std::pair<std::string, std::string> res;
    auto pos = vSlotName.find('_');  // slot names are like STAGE_NAME
    if (pos != std::string::npos) {
        res.first = vSlotName.substr(0U, pos);
        res.second = vSlotName.substr(pos + 1);
    }
    return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// NODESLOT CLASS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

BaseSlot::BaseSlot()
{
	pinID = sGetNewSlotId();
}

BaseSlot::BaseSlot(std::string vName)
{
	pinID = sGetNewSlotId();
	name = vName;
}

BaseSlot::BaseSlot(std::string vName, std::string vType)
{
	pinID = sGetNewSlotId();
	name = vName;
	slotType = vType;
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
	//stamp.typeStamp = ConvertUniformsTypeEnumToString(type);
}

BaseSlot::BaseSlot(std::string vName, std::string vType, bool vHideName)
{
	pinID = sGetNewSlotId();
	name = vName;
	slotType = vType;
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
	//stamp.typeStamp = ConvertUniformsTypeEnumToString(type);
	hideName = vHideName;
}

BaseSlot::BaseSlot(std::string vName, std::string vType, bool vHideName, bool vShowWidget)
{
	pinID = sGetNewSlotId();
	name = vName;
	slotType = vType;
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
	//stamp.typeStamp = ConvertUniformsTypeEnumToString(type);
	hideName = vHideName;
	showWidget = vShowWidget;
}

BaseSlot::~BaseSlot() = default;

void BaseSlot::Init()
{

}

void BaseSlot::Unit()
{
	// ici pas besoin du assert sur le m_This 
	// car BaseSlot peut etre isntancié à l'ancienne en copie local donc sans shared_ptr
	// donc pour gagner du temps on va checker le this, si expiré on va pas plus loins
	if (!m_This.expired())
	{
		if (!parentNode.expired())
		{
			auto parentNodePtr = parentNode.lock();
			if (parentNodePtr)
			{
				auto graph = parentNodePtr->GetParentNode();
				if (!graph.expired())
				{
					auto graphPtr = graph.lock();
					if (graphPtr)
					{
						graphPtr->BreakAllLinksConnectedToSlot(m_This);
					}
				}
			}
		}
	}
}

// name : toto, stamp : vec3(vec4) => result : vec3 toto(vec4)
std::string BaseSlot::GetFullStamp()
{
	std::string res;

	if (!name.empty() && !stamp.typeStamp.empty())
	{
		size_t par = stamp.typeStamp.find('(');
		if (par != std::string::npos)
		{
			res = stamp.typeStamp;
			res.insert(par, " " + name);
		}
	}

	return res;
}

void BaseSlot::NotifyConnectionChangeToParent(bool vConnected) // va contacter le parent pour lui dire que ce slot est connecté a un autre
{
	assert(!m_This.expired());
	if (!parentNode.expired())
	{
		auto parentNodePtr = parentNode.lock();
		if (parentNodePtr)
		{
			parentNodePtr->NotifyConnectionChangeOfThisSlot(m_This, vConnected);
		}
	}
}

bool BaseSlot::CanWeConnectToSlot(BaseSlotWeak vSlot)
{
	if (!parentNode.expired())
	{
		auto parentNodePtr = parentNode.lock();
		if (parentNodePtr)
		{
			assert(!m_This.expired());
			return parentNodePtr->CanWeConnectSlots(m_This, vSlot);
		}
	}

	return false;
}

uint32_t BaseSlot::GetSlotID() const
{
	return (uint32_t)pinID.Get();
}

std::vector<BaseSlotWeak> BaseSlot::InjectTypeInSlot(uType::uTypeEnum vType)
{
	std::vector<BaseSlotWeak> res;

	if (!parentNode.expired())
	{
		auto parentNodePtr = parentNode.lock();
		if (parentNodePtr)
		{
			assert(!m_This.expired());
			res = parentNodePtr->InjectTypeInSlot(m_This, vType);
		}
	}

	return res;
}

void BaseSlot::DrawContent(BaseNodeState* vBaseNodeState)
{
	if (vBaseNodeState && !hidden)
	{
		if (slotPlace == BaseSlot::PlaceEnum::INPUT)
		{
			nd::BeginPin(pinID, nd::PinKind::Input);
			{
				ImGui::BeginHorizontal(pinID.AsPointer());

				nd::PinPivotAlignment(ImVec2(0.0f, 0.5f));
				nd::PinPivotSize(ImVec2(0, 0));

				DrawSlot(vBaseNodeState, ImVec2(slotIconSize, slotIconSize));

				if (showWidget)
				{
					DrawInputWidget(vBaseNodeState);
				}
				if (!hideName)
				{
					ImGui::TextUnformatted(name.c_str());
				}

				ImGui::Spring(1);

				ImGui::EndHorizontal();
			}
			nd::EndPin();
		}
		else if (slotPlace == BaseSlot::PlaceEnum::OUTPUT)
		{
			nd::BeginPin(pinID, nd::PinKind::Output);
			{
				ImGui::BeginHorizontal(pinID.AsPointer());

				ImGui::Spring(1);

				if (!hideName)
				{
					ImGui::TextUnformatted(name.c_str());
				}
				if (showWidget)
				{
					DrawOutputWidget(vBaseNodeState);
				}

				nd::PinPivotAlignment(ImVec2(0.0f, 0.5f));
				nd::PinPivotSize(ImVec2(0, 0));

				DrawSlot(vBaseNodeState, ImVec2(slotIconSize, slotIconSize));

				ImGui::EndHorizontal();
			}
			nd::EndPin();
		}
	}
}

void BaseSlot::DrawSlot(BaseNodeState* vBaseNodeState, ImVec2 vSlotSize, ImVec2 vSlotOffset)
{
	if (vBaseNodeState)
	{
		ImGui::Dummy(vSlotSize);

		ImRect slotRect = ImGui::GetCurrentContext()->LastItemData.Rect;
		slotRect.Min += vSlotOffset;
		slotRect.Max += vSlotOffset;

		ImVec2 slotCenter = slotRect.GetCenter();
		pos = slotCenter;

		nd::PinPivotRect(slotCenter, slotCenter);
		nd::PinRect(slotRect.Min, slotRect.Max);

		highLighted = false;

		if (ImGui::IsRectVisible(vSlotSize))
		{
			if (!colorIsSet)
			{
				color = sGetSlotColors()->GetSlotColor(slotType);
				colorIsSet = true;
			}

			auto u_color = ImGui::GetColorU32(color);

			auto draw_list = ImGui::GetWindowDrawList();
			if (draw_list)
			{
				DrawBaseSlot(draw_list, slotCenter, vBaseNodeState, connected, u_color, u_color);
			}

			if (ImGui::IsItemHovered())
			{
				highLighted = true;

				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					MouseDoubleClickedOnSlot(ImGuiMouseButton_Left);
					BaseNode::SelectSlot_Callback(m_This, ImGuiMouseButton_Left);
				}
				else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Right))
				{
					MouseDoubleClickedOnSlot(ImGuiMouseButton_Right);
					BaseNode::SelectSlot_Callback(m_This, ImGuiMouseButton_Right);
				}
				else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Middle))
				{
					MouseDoubleClickedOnSlot(ImGuiMouseButton_Middle);
					BaseNode::SelectSlot_Callback(m_This, ImGuiMouseButton_Middle);
				}

				DrawSlotText(draw_list, slotCenter, vBaseNodeState, connected, u_color, u_color);
			}
		}
	}
}

bool BaseSlot::IsAnInput()
{
	return slotPlace == BaseSlot::PlaceEnum::INPUT;
}

bool BaseSlot::IsAnOutput()
{
	return slotPlace == BaseSlot::PlaceEnum::OUTPUT;
}

void BaseSlot::Notify(const NotifyEvent& vEvent, const BaseSlotWeak& vEmitterSlot, const BaseSlotWeak& vReceiverSlot)
{
    // one notification can be :
    // - from input to output : Front
    // - from output to input : Back
    // Front or Back will not been used in the same way

    if (vEmitterSlot.expired() || vEmitterSlot.lock() == m_This.lock())  // Front or Back
    {
        // we propagate the notification to the connected slots
        for (const auto& otherSlot : linkedSlots) {
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
        }
    } else  // receiving notification from other slots
    {
        // we treat the notification in herited slots
        TreatNotification(vEvent, vEmitterSlot, m_This);

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
        }
    }
}

void BaseSlot::SendNotification(const std::string& vSlotType, const NotifyEvent& vEvent) {
    auto nodePtr = parentNode.lock();
    if (nodePtr) {
        nodePtr->SendFrontNotification(vSlotType, vEvent);
    }
}

void BaseSlot::OnConnectEvent(BaseSlotWeak vOtherSlot) {
#ifdef _DEBUG
    // LogVarInfo("BaseSlot::OnConnectEvent catched by the slot \"%s\", some class not implement it. maybe its wanted", name.c_str());
#endif
}

void BaseSlot::OnDisConnectEvent(BaseSlotWeak vOtherSlot) {
#ifdef _DEBUG
    // LogVarInfo("BaseSlot::OnDisConnectEvent catched by the slot \"%s\", some class not implement it. maybe its wanted", name.c_str());
#endif
}

void BaseSlot::TreatNotification(const NotifyEvent& vEvent, const BaseSlotWeak& vEmitterSlot, const BaseSlotWeak& vReceiverSlot) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::TreatNotification catched by the slot \"%s\", some class not implement it. maybe its wanted", name.c_str());
#endif
}

void BaseSlot::SendFrontNotification(const NotifyEvent& vEvent) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::SendFrontNotification catched by the slot \"%s\", some class not implement it. maybe its wanted", name.c_str());
#endif
}

void BaseSlot::SendBackNotification(const NotifyEvent& vEvent) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::SendBackNotification catched by the slot \"%s\", some class not implement it. maybe its wanted", name.c_str());
#endif
}

void BaseSlot::MouseDoubleClickedOnSlot(const ImGuiMouseButton& vMouseButton) {
#ifdef _DEBUG
    LogVarInfo("BaseSlot::MouseDoubleClickedOnSlot catched by the slot \"%s\", some class not implement it. maybe its wanted", name.c_str());
#endif
}

bool BaseSlot::RemoveConnectedSlot(BaseSlotWeak vOtherSlot) {
    auto ptr = vOtherSlot.lock();
    if (ptr) {
        auto it = linkedSlots.begin();
        for (; it != linkedSlots.end(); ++it) {
            auto it_ptr = it->lock();
            if (it_ptr && it_ptr == ptr) {
                break;
            }
        }

        // found we erase it
        if (it != linkedSlots.end()) {
            auto it_ptr = it->lock();
            if (it_ptr) {
                LogVarDebugError("slot(%u) link break with slot(%u)", GetSlotID(), it_ptr->GetSlotID());
            }

            linkedSlots.erase(it);

            if (linkedSlots.empty()) {
                connected = false;
            }

            return true;
        } else {
            CTOOL_DEBUG_BREAK;
            LogVarError("Connected slot not found");
        }
    }

    return false;
}

void BaseSlot::DrawDebugInfos() {
    ImGui::Text("--------------------");
    ImGui::Text("Slot %s", name.c_str());
    ImGui::Text(IsAnInput() ? "Input" : "Output");
    ImGui::Text("Count connections : %u", (uint32_t)linkedSlots.size());
}

//////////////////////////////////////////////////////////////
//// PRIVATE /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void BaseSlot::DrawInputWidget(BaseNodeState* vBaseNodeState) {
    if (vBaseNodeState && !parentNode.expired()) {
        assert(!m_This.expired());
        auto ptr = parentNode.lock();
        if (ptr) {
            ptr->DrawInputWidget(vBaseNodeState, m_This);
        }
    }
}

void BaseSlot::DrawOutputWidget(BaseNodeState* vBaseNodeState) {
    if (vBaseNodeState && !parentNode.expired()) {
        assert(!m_This.expired());
        auto ptr = parentNode.lock();
        if (ptr) {
            ptr->DrawOutputWidget(vBaseNodeState, m_This);
        }
    }
}

void BaseSlot::DrawSlotText(ImDrawList* vDrawList, ImVec2 vCenter, BaseNodeState* vBaseNodeState, bool vConnected, ImU32 vColor, ImU32 vInnerColor) {
    if (vBaseNodeState) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        if (draw_list) {
            std::string slotUType;
            std::string slotName;

            if (slotPlace == BaseSlot::PlaceEnum::INPUT) {
                slotName = slotType;

                if (vBaseNodeState->debug_mode) {
                    slotName = "in " + slotUType + " links(";

                    int idx = 0;
                    for (auto link : linkedSlots) {
                        auto linkPtr = link.lock();
                        if (linkPtr) {
                            if (idx > 0)
                                slotName += ", ";

                            if (!linkPtr->parentNode.expired()) {
                                auto parentNodePtr = linkPtr->parentNode.lock();
                                if (parentNodePtr) {
                                    slotName += parentNodePtr->name;
                                }
                            }
                        }

                        idx++;
                    }

                    slotName += ")";
                }
                size_t len = slotName.length();
                if (len > 0) {
                    const char* beg = slotName.c_str();
                    ImVec2 txtSize = ImGui::CalcTextSize(beg);
                    ImVec2 min = ImVec2(pos.x - slotIconSize * 0.5f - txtSize.x, pos.y - slotIconSize * 0.5f);
                    ImVec2 max = min + ImVec2(txtSize.x, slotIconSize);
                    ImGui::RenderFrame(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
                    draw_list->AddText(ImVec2(min.x, pos.y - txtSize.y * 0.55f), ImColor(200, 200, 200, 255), beg);
                }
            } else if (slotPlace == BaseSlot::PlaceEnum::OUTPUT) {
                slotName = slotType;

                if (vBaseNodeState->debug_mode) {
                    slotName = "links(" + ez::toStr(linkedSlots.size()) + ")" + slotUType + " out";
                }
                size_t len = slotName.length();
                if (len > 0) {
                    const char* beg = slotName.c_str();
                    ImVec2 txtSize = ImGui::CalcTextSize(beg);
                    ImVec2 min = ImVec2(pos.x + slotIconSize * 0.5f, pos.y - slotIconSize * 0.5f);
                    ImVec2 max = min + ImVec2(txtSize.x, slotIconSize);
                    ImGui::RenderFrame(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
                    draw_list->AddText(ImVec2(min.x, pos.y - txtSize.y * 0.55f), ImColor(200, 200, 200, 255), beg);
                }
            }
        }
    }
}
void BaseSlot::DrawBaseSlot(ImDrawList* vDrawList, ImVec2 vCenter, BaseNodeState* vBaseNodeState, bool vConnected, ImU32 vColor, ImU32 vInnerColor) {
    UNUSED(vInnerColor);
    UNUSED(vConnected);

    if (vDrawList) {
        const auto slotRadius = vBaseNodeState->graphStyle.SLOT_RADIUS;

        vDrawList->AddNgonFilled(vCenter, slotRadius, vColor, 24);

        if (ImGui::IsItemHovered()) {
            ImVec4 _color = ImGui::ColorConvertU32ToFloat4(vColor);
            _color.w = 0.5f;
            vDrawList->AddNgon(vCenter, slotRadius + 2.0f, ImGui::GetColorU32(_color), 24, 2.5f);
        }

        auto slotOutputMouseLeftPtr = BaseSlot::sSlotGraphOutputMouseLeft.lock();  // blue
        if (slotOutputMouseLeftPtr && slotOutputMouseLeftPtr == m_This.lock()) {
            vDrawList->AddNgon(vCenter, slotRadius + 2.0f, ImGui::GetColorU32(BaseSlot::sSlotGraphOutputMouseLeftColor), 24, 2.5f);
        }

        auto slotOutputMouseMiddlePtr = BaseSlot::sSlotGraphOutputMouseMiddle.lock();  // green
        if (slotOutputMouseMiddlePtr && slotOutputMouseMiddlePtr == m_This.lock()) {
            vDrawList->AddNgon(vCenter, slotRadius + 2.0f, ImGui::GetColorU32(BaseSlot::sSlotGraphOutputMouseMiddleColor), 24, 2.5f);
        }

        auto slotOutputMouseRightPtr = BaseSlot::sSlotGraphOutputMouseRight.lock();  // red
        if (slotOutputMouseRightPtr && slotOutputMouseRightPtr == m_This.lock()) {
            vDrawList->AddNgon(vCenter, slotRadius + 2.0f, ImGui::GetColorU32(BaseSlot::sSlotGraphOutputMouseRightColor), 24, 2.5f);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

std::string BaseSlot::getXml(const std::string& vOffset, const std::string& vUserDatas) {
    std::string res;

    res += vOffset +
        ez::toStr(
               "<slot index=\"%u\" name=\"%s\" type=\"%s\" place=\"%s\" id=\"%u\"/>\n",
               index,
               name.c_str(),
               slotType.c_str(),
               sGetStringFromBaseSlotPlaceEnum(slotPlace).c_str(),
               (uint32_t)GetSlotID());

    return res;
}

bool BaseSlot::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) {
    // The value of this child identifies the name of this element
    std::string strName;
    std::string strValue;
    std::string strParentName;

    strName = vElem->Value();
    if (vElem->GetText())
        strValue = vElem->GetText();
    if (vParent != nullptr)
        strParentName = vParent->Value();

    if (strName == "slot" && strParentName == "node") {
        uint32_t _index = 0U;
        std::string _name;
        std::string _type = "NONE";
        BaseSlot::PlaceEnum _place = BaseSlot::PlaceEnum::NONE;
        uint32_t _pinId = 0U;

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
                _pinId = ez::ivariant(attValue).GetU();
        }

        if (index == _index && slotType == _type && slotPlace == _place && !idAlreadySetbyXml) {
            pinID = _pinId;
            idAlreadySetbyXml = true;

            // pour eviter que des slots aient le meme id qu'un nodePtr
            BaseNode::freeNodeId = ez::maxi<uint32_t>(BaseNode::freeNodeId, (uint32_t)GetSlotID()) + 1U;

            return false;
        }
    }

    return true;
}
*/