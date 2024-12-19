/*
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

#include <SoGLSL/Headers/SoGLSLDefs.h>
#include <SoGLSL/Graph/Base/BaseNode.h>
#include <SoGLSL/Graph/Slots/NodeSlotCodeOutput.h>
#include <SoGLSL/Interfaces/SlotCodeInputInterface.h>
#include <SoGLSL/Interfaces/SlotCodeOutputInterface.h>

#include <utility>
static const float slotIconSize = 15.0f;

//////////////////////////////////////////////////////////////////////////////////////////////
//// STATIC //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

NodeSlotCodeOutputPtr NodeSlotCodeOutput::Create(NodeSlotCodeOutput vSlot)
{
	auto res = std::make_shared<NodeSlotCodeOutput>(vSlot);
	res->m_This = res;
	return res;
}

NodeSlotCodeOutputPtr NodeSlotCodeOutput::Create(const std::string& vName, const std::string& vType)
{
	auto res = std::make_shared<NodeSlotCodeOutput>(vName, vType);
	res->m_This = res;
	return res;
}

NodeSlotCodeOutputPtr NodeSlotCodeOutput::Create(const std::string& vName, const std::string& vType, const bool& vHideName)
{
	auto res = std::make_shared<NodeSlotCodeOutput>(vName, vType, vHideName);
	res->m_This = res;
	return res;
}

NodeSlotCodeOutputPtr NodeSlotCodeOutput::Create(const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget)
{
	auto res = std::make_shared<NodeSlotCodeOutput>(vName, vType, vHideName, vShowWidget);
	res->m_This = res;
	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// NODESLOT CLASS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

NodeSlotCodeOutput::NodeSlotCodeOutput()
	: NodeSlotOutput("", "")
{
	pinID = sGetNewSlotId();
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
}

NodeSlotCodeOutput::NodeSlotCodeOutput(const std::string& vName, const std::string& vType)
	: NodeSlotOutput(vName, vType)
{
	pinID = sGetNewSlotId();
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
}

NodeSlotCodeOutput::NodeSlotCodeOutput(const std::string& vName, const std::string& vType, const bool& vHideName)
	: NodeSlotOutput(vName, vType, vHideName)
{
	pinID = sGetNewSlotId();
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
}

NodeSlotCodeOutput::NodeSlotCodeOutput(const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget)
	: NodeSlotOutput(vName, vType, vHideName, vShowWidget)
{
	pinID = sGetNewSlotId();
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
}

NodeSlotCodeOutput::~NodeSlotCodeOutput() = default;

void NodeSlotCodeOutput::Init()
{
	
}

void NodeSlotCodeOutput::Unit()
{
	// ici pas besoin du assert sur le m_This 
	// car NodeSlotCodeOutput peut etre instancié à l'ancienne en copie local donc sans shared_ptr
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
						//graphPtr->DisConnectSlot(m_This);
					}
				}
			}
		}
	}
}

void NodeSlotCodeOutput::SendFrontNotification(const NotifyEvent& vEvent)
{
	if (vEvent == CodeUpdateDone)
	{
		SendNotification(slotType, vEvent);
	}
}

void NodeSlotCodeOutput::DrawDebugInfos()
{
	ImGui::Text("--------------------");
	ImGui::Text("Slot %s", name.c_str());
	ImGui::Text(IsAnInput() ? "Input" : "Output");
	ImGui::Text("Count connections : %u", (uint32_t)linkedSlots.size());
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

std::string NodeSlotCodeOutput::getXml(const std::string& vOffset, const std::string& /*vUserDatas*/)
{
	std::string res;

	res += vOffset + ez::toStr("<slot index=\"%u\" name=\"%s\" type=\"%s\" place=\"%s\" id=\"%u\"/>\n",
		index,
		name.c_str(),
		slotType.c_str(),
		NodeSlot::sGetStringFromNodeSlotPlaceEnum(slotPlace).c_str(),
		(uint32_t)pinID.Get());

	return res;
}

bool NodeSlotCodeOutput::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& /*vUserDatas*/)
{
	// The value of this child identifies the name of this element
	std::string strName;
	std::string strValue;
	std::string strParentName;

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	if (strName == "slot" && strParentName == "node")
	{
		uint32_t _index = 0U;
		std::string _name;
		std::string _type = "NONE";
		auto _place = NodeSlot::PlaceEnum::NONE;
		uint32_t _pinId = 0U;

		for (const tinyxml2::XMLAttribute* attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
		{
			std::string attName = attr->Name();
			std::string attValue = attr->Value();

			if (attName == "index")
				_index = ez::ivariant(attValue).GetU();
			else if (attName == "name")
				_name = attValue;
			else if (attName == "type")
				_type = attValue;
			else if (attName == "place")
				_place = NodeSlot::sGetNodeSlotPlaceEnumFromString(attValue);
			else if (attName == "id")
				_pinId = ez::ivariant(attValue).GetU();
		}

		if (index == _index &&
			slotType == _type && 
			slotPlace == _place && 
			!idAlreadySetbyXml)
		{
			pinID = _pinId;
			idAlreadySetbyXml = true;

			// pour eviter que des slots aient le meme id qu'un nodePtr
			BaseNode::freeNodeId = ez::maxi<uint32_t>(BaseNode::freeNodeId, (uint32_t)pinID.Get());

			return false;
		}
	}	

	return true;
}