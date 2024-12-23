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

#include <SoGLSL/Graph/Base/BaseSlotOutput.h>

#include <utility>
#include <SoGLSL/Graph/Base/BaseNode.h>
static const float slotIconSize = 15.0f;

//////////////////////////////////////////////////////////////////////////////////////////////
//// STATIC //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

BaseSlotOutputPtr BaseSlotOutput::Create()
{
	auto res = std::make_shared<BaseSlotOutput>();
	res->m_This = res;
	return res;
}

BaseSlotOutputPtr BaseSlotOutput::Create(const std::string& vName)
{
	auto res = std::make_shared<BaseSlotOutput>(vName);
	res->m_This = res;
	return res;
}

BaseSlotOutputPtr BaseSlotOutput::Create(const std::string& vName, const std::string& vType)
{
	auto res = std::make_shared<BaseSlotOutput>(vName, vType);
	res->m_This = res;
	return res;
}

BaseSlotOutputPtr BaseSlotOutput::Create(const std::string& vName, const std::string& vType, const bool& vHideName)
{
	auto res = std::make_shared<BaseSlotOutput>(vName, vType, vHideName);
	res->m_This = res;
	return res;
}

BaseSlotOutputPtr BaseSlotOutput::Create(const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget)
{
	auto res = std::make_shared<BaseSlotOutput>(vName, vType, vHideName, vShowWidget);
	res->m_This = res;
	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// NODESLOT CLASS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

BaseSlotOutput::BaseSlotOutput()
	: BaseSlot()
{
	pinID = sGetNewSlotId();
	slotPlace = BaseSlot::PlaceEnum::OUTPUT;
}

BaseSlotOutput::BaseSlotOutput(const std::string& vName)
	: BaseSlot(vName)
{
	pinID = sGetNewSlotId();
	slotPlace = BaseSlot::PlaceEnum::OUTPUT;
}

BaseSlotOutput::BaseSlotOutput(const std::string& vName, const std::string& vType)
	: BaseSlot(vName, vType)
{
	pinID = sGetNewSlotId();
	slotPlace = BaseSlot::PlaceEnum::OUTPUT;
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
}

BaseSlotOutput::BaseSlotOutput(const std::string& vName, const std::string& vType, const bool& vHideName)
	: BaseSlot(vName, vType, vHideName)
{
	pinID = sGetNewSlotId();
	slotPlace = BaseSlot::PlaceEnum::OUTPUT;
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
}

BaseSlotOutput::BaseSlotOutput(const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget)
	: BaseSlot(vName, vType, vHideName, vShowWidget)
{
	pinID = sGetNewSlotId();
	slotPlace = BaseSlot::PlaceEnum::OUTPUT;
	color = sGetSlotColors()->GetSlotColor(slotType);
	colorIsSet = true;
}

BaseSlotOutput::~BaseSlotOutput() = default;

void BaseSlotOutput::Init()
{
	
}

void BaseSlotOutput::Unit()
{
	// ici pas besoin du assert sur le m_This 
	// car BaseSlotOutput peut etre isntancié à l'ancienne en copie local donc sans shared_ptr
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

void BaseSlotOutput::DrawDebugInfos()
{
	ImGui::Text("--------------------");
	ImGui::Text("Slot %s", name.c_str());
	ImGui::Text(IsAnInput() ? "Input" : "Output");
	ImGui::Text("Count connections : %u", (uint32_t)linkedSlots.size());
}
