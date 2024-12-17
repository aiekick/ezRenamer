// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "NodeSlot.h"

#include <utility>
#include "BaseNode.h"

static const float slotIconSize = 15.0f;

//////////////////////////////////////////////////////////////////////////////////////////////
//// STATIC //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

size_t NodeSlot::GetNewSlotId()
{
	#define SLOT_ID_OFFSET 100000
	return SLOT_ID_OFFSET + (++BaseNode::freeNodeId);
}

std::string NodeSlot::GetStringFromNodeSlotTypeEnum(const NodeSlotTypeEnum& vNodeSlotTypeEnum)
{
	static std::array<std::string, (uint32_t)NodeSlotTypeEnum::NODE_SLOT_TYPE_Count> NodeTypeString = {
		"NONE",
		"DEFAULT", // le node stage (qui peut contenir d'autre graph)
	};
	if (vNodeSlotTypeEnum != NodeSlotTypeEnum::NODE_SLOT_TYPE_Count)
		return NodeTypeString[(int)vNodeSlotTypeEnum];
	LogVarDebug("Error, one NodeSlotTypeEnum have no corresponding string, return \"None\"");
	return "NONE";
}
ImVec4 NodeSlot::GetSlotColorAccordingToType(uType::uTypeEnum vType)
{
	ImVec4 res = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);

	switch (vType)
	{
	case uType::uTypeEnum::U_FLOAT:
	case uType::uTypeEnum::U_VEC2:
	case uType::uTypeEnum::U_VEC3:
	case uType::uTypeEnum::U_VEC4:
	case uType::uTypeEnum::U_FLOAT_ARRAY:
	case uType::uTypeEnum::U_VEC2_ARRAY:
	case uType::uTypeEnum::U_VEC3_ARRAY:
	case uType::uTypeEnum::U_VEC4_ARRAY:
		res = ImVec4(0.5f, 1.0f, 0.5f, 1.0f); // float : vert foncé
		break;
	case uType::uTypeEnum::U_INT:
	case uType::uTypeEnum::U_IVEC2:
	case uType::uTypeEnum::U_IVEC3:
	case uType::uTypeEnum::U_IVEC4:
	case uType::uTypeEnum::U_INT_ARRAY:
		res = ImVec4(0.2f, 1.0f, 0.2f, 1.0f); // int : vert clair
		break;
	case uType::uTypeEnum::U_BOOL:
	case uType::uTypeEnum::U_BVEC2:
	case uType::uTypeEnum::U_BVEC3:
	case uType::uTypeEnum::U_BVEC4:
		res = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); // Bool : rouge foncé
		break;
	case uType::uTypeEnum::U_IMAGE1D:
	case uType::uTypeEnum::U_IMAGE2D:
	case uType::uTypeEnum::U_IMAGE3D:
		res = ImVec4(0.5f, 0.5f, 1.0f, 1.0f); // Image : bleu foncé
		break;
	case uType::uTypeEnum::U_SAMPLER1D:
	case uType::uTypeEnum::U_SAMPLER1D_ARRAY:
	case uType::uTypeEnum::U_SAMPLER2D:
	case uType::uTypeEnum::U_SAMPLER2D_ARRAY:
	case uType::uTypeEnum::U_SAMPLER3D:
	case uType::uTypeEnum::U_SAMPLER3D_ARRAY:
	case uType::uTypeEnum::U_SAMPLER2D_TEXTUREARRAY:
	case uType::uTypeEnum::U_SAMPLERCUBE:
		res = ImVec4(0.2f, 0.2f, 1.0f, 1.0f); // Sampler : bleu clair 
		break;
	case uType::uTypeEnum::U_MAT2:
	case uType::uTypeEnum::U_MAT3:
	case uType::uTypeEnum::U_MAT4:
		res = ImVec4(0.75f, 1.0f, 0.5f, 1.0f); // vert-rouge foncé
		break;
	case uType::uTypeEnum::U_STRUCT:
	case uType::uTypeEnum::U_PROGRAM:
		res = ImVec4(1.0f, 0.5f, 0.2f, 1.0f); // jaune clair
		break;
	case uType::uTypeEnum::U_VOID:
	case uType::uTypeEnum::U_TYPE:
	case uType::uTypeEnum::U_VEC:
	case uType::uTypeEnum::U_MAT:
	case uType::uTypeEnum::U_TEXT:
		res = ImVec4(0.8f, 0.8f, 0.0f, 1.0f); // base : jaune cassé
		break;
	case uType::uTypeEnum::U_FLOW:
		res = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // blanc cassé
		break;
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// NODESLOT CLASS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

NodeSlot::NodeSlot()
{
	

	pinID = GetNewSlotId();
}

NodeSlot::NodeSlot(std::string vName)
{
	

	pinID = GetNewSlotId();
	name = vName;
}

NodeSlot::NodeSlot(std::string vName, uType::uTypeEnum vType)
{
	

	pinID = GetNewSlotId();
	name = vName;
	type = vType;
	color = GetSlotColorAccordingToType(type);
	colorIsSet = true;
	stamp.typeStamp = ConvertUniformsTypeEnumToString(type);
}

NodeSlot::NodeSlot(std::string vName, uType::uTypeEnum vType, bool vHideName)
{
	

	pinID = GetNewSlotId();
	name = vName;
	type = vType;
	color = GetSlotColorAccordingToType(type);
	colorIsSet = true;
	stamp.typeStamp = ConvertUniformsTypeEnumToString(type);
	hideName = vHideName;
}

NodeSlot::NodeSlot(std::string vName, uType::uTypeEnum vType, bool vHideName, bool vShowWidget)
{
	

	pinID = GetNewSlotId();
	name = vName;
	type = vType;
	color = GetSlotColorAccordingToType(type);
	colorIsSet = true;
	stamp.typeStamp = ConvertUniformsTypeEnumToString(type);
	hideName = vHideName;
	showWidget = vShowWidget;
}

NodeSlot::~NodeSlot()
{
	

	//Unit();
}

void NodeSlot::Init()
{
	
}

void NodeSlot::Unit()
{
	

	// ici pas besoin du assert sur le m_This 
	// car NodeSlot peut etre isntancié à l'ancienne en copie local donc sans shared_ptr
	// donc pour gagner du temps on va checker le this, si expiré on va pas plus loins
	if (!m_This.expired())
	{
		if (!parentNode.expired())
		{
			auto parentNodePtr = parentNode.lock();
			if (parentNodePtr)
			{
				auto graph = parentNodePtr->m_ParentNode;
				if (!graph.expired())
				{
					auto graphPtr = graph.lock();
					if (graphPtr)
					{
						graphPtr->DisConnectSlot(m_This);
					}
				}
			}
		}
	}
}

// name : toto, stamp : vec3(vec4) => result : vec3 toto(vec4)
std::string NodeSlot::GetFullStamp()
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

void NodeSlot::NotifyConnectionChangeToParent(bool vConnected) // va contacter le parent pour lui dire que ce slot est connecté a un autre
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

bool NodeSlot::CanWeConnectToSlot(std::weak_ptr<NodeSlot> vSlot)
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

std::vector<std::weak_ptr<NodeSlot>> NodeSlot::InjectTypeInSlot(uType::uTypeEnum vType)
{
	std::vector<std::weak_ptr<NodeSlot>> res;

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

void NodeSlot::DrawContent(BaseNodeStateStruct *vCanvasState)
{
	if (vCanvasState && !hidden)
	{
		if (slotPlace == NodeSlotPlaceEnum::NODE_SLOT_PLACE_INPUT)
		{
			/*BeginPin(pinID, PinKiInput);
			{
				ImGui::BeginHorizontal(pinID);
				
				PinPivotAlignment(ImVec2(0.0f, 0.5f));
				PinPivotSize(ImVec2(0, 0));
				
				DrawSlot(vCanvasState, ImVec2(slotIconSize, slotIconSize));

				ImGui::Spring(0);
				
				if (showWidget)
				{
					ImGui::Spring(0);
					DrawInputWidget(vCanvasState);
				}
				if (!hideName)
				{
					ImGui::Spring(0);
					ImGui::TextUnformatted(name.c_str());
				}

				ImGui::EndHorizontal();
			}
			EndPin();*/
		}
		else if (slotPlace == NodeSlotPlaceEnum::NODE_SLOT_PLACE_OUTPUT)
		{
			/*BeginPin(pinID, PinKiOutput);
			{
				ImGui::BeginHorizontal(pinID);

				if (!hideName)
				{
					ImGui::TextUnformatted(name.c_str());
					ImGui::Spring(0);
				}
				if (showWidget)
				{
					DrawOutputWidget(vCanvasState);
					ImGui::Spring(0);
				}

				PinPivotAlignment(ImVec2(1.0f, 0.5f));
				PinPivotSize(ImVec2(0, 0));

				ImGui::Spring(0);
				
				DrawSlot(vCanvasState, ImVec2(slotIconSize, slotIconSize));
				
				ImGui::EndHorizontal();
			}
			EndPin();*/
		}
	}
}

void NodeSlot::DrawSlot(BaseNodeStateStruct *vCanvasState, ImVec2 vSlotSize, ImVec2 vSlotOffset)
{
	if (vCanvasState)
	{
		ImGui::Dummy(vSlotSize);

		ImGuiContext& g = *GImGui;
		ImRect slotRect = g.LastItemData.Rect;
		slotRect.Min += vSlotOffset;
		slotRect.Max += vSlotOffset;

		ImVec2 slotCenter = slotRect.GetCenter();
		pos = slotCenter;

		/*PinPivotRect(slotCenter, slotCenter);
		PinRect(slotRect.Min, slotRect.Max);*/

		highLighted = false;

		if (ImGui::IsRectVisible(vSlotSize))
		{
			auto draw_list = ImGui::GetWindowDrawList();

			if (draw_list)
			{
				if (!colorIsSet)
				{
					color = GetSlotColorAccordingToType(type);
					colorIsSet = true;
				}

				if (slotType == NodeSlotTypeEnum::NODE_SLOT_TYPE_DEFAULT)
				{
					DrawSlot(draw_list, slotCenter, vCanvasState->graphStyle.PinRadius, 
						connected, ImGui::GetColorU32(color),
						ImGui::GetColorU32(ImVec4(1,1,0,1)));
				}
			}

			if (ImGui::IsItemHovered())
			{
				highLighted = true;

				DrawSlotText(vCanvasState);
			}
		}
	}
}

void NodeSlot::DrawInputWidget(BaseNodeStateStruct *vCanvasState)
{
	if (vCanvasState && !parentNode.expired())
	{
		assert(!m_This.expired());
		auto ptr = parentNode.lock();
		if (ptr)
		{
			ptr->DrawInputWidget(vCanvasState, m_This);
		}
	}
}

void NodeSlot::DrawOutputWidget(BaseNodeStateStruct *vCanvasState)
{
	if (vCanvasState && !parentNode.expired())
	{
		assert(!m_This.expired());
		auto ptr = parentNode.lock();
		if (ptr)
		{
			ptr->DrawOutputWidget(vCanvasState, m_This);
		}
	}
}

void NodeSlot::DrawSlotText(BaseNodeStateStruct *vCanvasState)
{
	if (vCanvasState)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		if (draw_list)
		{
			std::string slotUType;
			std::string slotName;

			if (slotPlace == NodeSlotPlaceEnum::NODE_SLOT_PLACE_INPUT)
			{
				if (slotType == NodeSlotTypeEnum::NODE_SLOT_TYPE_DEFAULT)
				{
					slotName = stamp.typeStamp;
				}
				else
				{
					slotUType = uType::ConvertUniformsTypeEnumToString(type);
					slotName = name + " (" + slotUType + ")";
				}
				if (vCanvasState->debug_mode)
				{
					slotName = "in " + slotUType + " links(";

					int idx = 0;
					for (auto link : links)
					{
						if (!link.expired())
						{
							auto linkPtr = link.lock();
							if (linkPtr)
							{
								if (idx > 0)
									slotName += ", ";

								if (!linkPtr->parentNode.expired())
								{
									auto parentNodePtr = linkPtr->parentNode.lock();
									if (parentNodePtr)
									{
										slotName += parentNodePtr->name;
									}
								}
							}
						}

						idx++;
					}

					slotName += ")";
				}
				size_t len = slotName.length();
				if (len > 0)
				{
					const char *beg = slotName.c_str();
					ImVec2 txtSize = ImGui::CalcTextSize(beg);
					ImVec2 min = ImVec2(pos.x - slotIconSize * 0.5f - txtSize.x, pos.y - slotIconSize * 0.5f);
					ImVec2 max = min + ImVec2(txtSize.x, slotIconSize);
					ImGui::RenderFrame(min, max, ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
					draw_list->AddText(ImVec2(min.x, pos.y - txtSize.y * 0.55f), ImColor(200, 200, 200, 255), beg);
				}
			}
			else if (slotPlace == NodeSlotPlaceEnum::NODE_SLOT_PLACE_OUTPUT)
			{
				if (slotType == NodeSlotTypeEnum::NODE_SLOT_TYPE_DEFAULT)
				{
					slotName = stamp.typeStamp;
				}
				else
				{
					slotUType = uType::ConvertUniformsTypeEnumToString(type);
					slotName = "(" + slotUType + ")";
				}
				if (vCanvasState->debug_mode)
				{
					slotName = "links(" + ct::toStr(links.size()) + ")" + slotUType + " out";
				}
				size_t len = slotName.length();
				if (len > 0)
				{
					const char *beg = slotName.c_str();
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

void NodeSlot::DrawSlot(ImDrawList *vDrawList, ImVec2 vCenter, float vSlotRadius, bool vConnected, ImU32 vColor, ImU32 vInnerColor) const
{
	UNUSED(vInnerColor);
	UNUSED(vConnected);

	if (vDrawList)
	{
		if (slotPlace == NodeSlotPlaceEnum::NODE_SLOT_PLACE_INPUT)
		{
			vDrawList->AddRectFilled(
				ImVec2(vCenter.x - vSlotRadius, vCenter.y - vSlotRadius),
				ImVec2(vCenter.x + vSlotRadius, vCenter.y + vSlotRadius),
				ImGui::GetColorU32(vColor),
				vSlotRadius, ImDrawFlags_RoundCornersRight);
		}
		else if (slotPlace == NodeSlotPlaceEnum::NODE_SLOT_PLACE_OUTPUT)
		{
			vDrawList->AddRectFilled(
				ImVec2(vCenter.x - vSlotRadius, vCenter.y - vSlotRadius),
				ImVec2(vCenter.x + vSlotRadius, vCenter.y + vSlotRadius),
				ImGui::GetColorU32(vColor),
				vSlotRadius, ImDrawFlags_RoundCornersLeft);
		}
	}
}
