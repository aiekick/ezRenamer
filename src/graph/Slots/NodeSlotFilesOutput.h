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
#include <SoGLSL/Graph/Base/NodeSlotOutput.h>

class NodeSlotCodeOutput;
typedef std::weak_ptr<NodeSlotCodeOutput> NodeSlotCodeOutputWeak;
typedef std::shared_ptr<NodeSlotCodeOutput> NodeSlotCodeOutputPtr;

class NodeSlotCodeOutput : public NodeSlotOutput {
public:
    static NodeSlotCodeOutputPtr Create(NodeSlotCodeOutput vSlot);
    static NodeSlotCodeOutputPtr Create(const std::string& vName, const std::string& vType);
    static NodeSlotCodeOutputPtr Create(const std::string& vName, const std::string& vType, const bool& vHideName);
    static NodeSlotCodeOutputPtr Create(
        const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget);

public:
    explicit NodeSlotCodeOutput();
    explicit NodeSlotCodeOutput(const std::string& vName, const std::string& vType);
    explicit NodeSlotCodeOutput(const std::string& vName, const std::string& vType, const bool& vHideName);
    explicit NodeSlotCodeOutput(
        const std::string& vName, const std::string& vType, const bool& vHideName, const bool& vShowWidget);
    ~NodeSlotCodeOutput();

    void Init();
    void Unit();

    void SendFrontNotification(const NotifyEvent& vEvent) override;

    void DrawDebugInfos();

    std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
    bool setFromXml(
        tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
};
