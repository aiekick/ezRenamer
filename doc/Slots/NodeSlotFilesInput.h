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

#pragma once

#include <SoGLSL/Graph/Graph.h>
#include <SoGLSL/Graph/Base/BaseSlotInput.h>

class BaseSlotCodeInput;
typedef std::weak_ptr<BaseSlotCodeInput> BaseSlotCodeInputWeak;
typedef std::shared_ptr<BaseSlotCodeInput> BaseSlotCodeInputPtr;

class BaseSlotCodeInput : 
	public BaseSlotInput
{
public:
	static BaseSlotCodeInputPtr Create(BaseSlotCodeInput vSlot);
	static BaseSlotCodeInputPtr Create(const std::string& vName, const std::string& vType);
	static BaseSlotCodeInputPtr Create(const std::string& vName, const std::string& vType, const bool& vHideName);
	static BaseSlotCodeInputPtr Create(const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget);

public:
	explicit BaseSlotCodeInput();
	explicit BaseSlotCodeInput(const std::string& vName, const std::string& vType);
	explicit BaseSlotCodeInput(const std::string& vName, const std::string& vType, const bool& vHideName);
	explicit BaseSlotCodeInput(const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget);
	~BaseSlotCodeInput();

	void Init();
	void Unit();

    void OnConnectEvent(BaseSlotWeak vOtherSlot) override;
    void OnDisConnectEvent(BaseSlotWeak vOtherSlot) override;

	void TreatNotification(
		NotifyEvent vEvent,
		const BaseSlotWeak& vEmitterSlot = BaseSlotWeak(),
		const BaseSlotWeak& vReceiverSlot = BaseSlotWeak());

	void DrawDebugInfos();

	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
};