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
#include <SoGLSL/Graph/Base/BaseSlotOutput.h>

class BaseSlotCodeOutput;
typedef std::weak_ptr<BaseSlotCodeOutput> BaseSlotCodeOutputWeak;
typedef std::shared_ptr<BaseSlotCodeOutput> BaseSlotCodeOutputPtr;

class BaseSlotCodeOutput : public BaseSlotOutput {
public:
    static BaseSlotCodeOutputPtr Create(BaseSlotCodeOutput vSlot);
    static BaseSlotCodeOutputPtr Create(const std::string& vName, const std::string& vType);
    static BaseSlotCodeOutputPtr Create(const std::string& vName, const std::string& vType, const bool& vHideName);
    static BaseSlotCodeOutputPtr Create(
        const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget);

public:
    explicit BaseSlotCodeOutput();
    explicit BaseSlotCodeOutput(const std::string& vName, const std::string& vType);
    explicit BaseSlotCodeOutput(const std::string& vName, const std::string& vType, const bool& vHideName);
    explicit BaseSlotCodeOutput(
        const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget);
    ~BaseSlotCodeOutput();

    void Init();
    void Unit();

    void SendFrontNotification(const NotifyEvent& vEvent) override;

    void DrawDebugInfos();

    std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
    bool setFromXml(
        tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
};
