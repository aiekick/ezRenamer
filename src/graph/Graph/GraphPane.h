/*
 * Copyright 2020 Stephane Cuillerdier (aka Aiekick)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <Panes/Abstract/AbstractPane.h>

#include <imgui/imgui.h>

#include <stdint.h>
#include <string>
#include <map>

#include <Graph/Base/BaseNode.h>
#include <ctools/ConfigAbstract.h>

class ProjectFile;
class GraphPane : public conf::ConfigAbstract, public AbstractPane
{
private:
	std::shared_ptr<BaseNode> m_RootNode;
	// nodeId, visibility
	typedef std::pair<std::weak_ptr<BaseNode>, bool> GraphVisibilityPair;
	std::vector<GraphVisibilityPair> m_GraphPanes;
	std::weak_ptr<BaseNode> m_LastFocusedGraph;

public:
	bool Init() override;
	void Unit() override;
	int DrawPanes(int vWidgetId, std::string vUserDatas) override;
	void DrawDialogsAndPopups(std::string vUserDatas) override;
	int DrawWidgets(int vWidgetId, std::string vUserDatas) override;
	void SelectNode(ct::cWeak<BaseNode> vNode) override;

	void DrawDebugInfos();
	void DrawProperties();

	void Clear();

	void DeleteNodesIfAnys();

	void AddGraphPane(std::weak_ptr<BaseNode> vNodeGraphToShow);
	void RemoveGraphPane(std::weak_ptr<BaseNode> vNodeGraphToShow);
	void ClearGraphPanes();

	void FinalizeGraphLoading();

private:
	bool DrawGraph(std::weak_ptr<BaseNode> vNode, bool &vCanShow, bool vRootNode, size_t vInitialPanesCount);

public:
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

public: // singleton
	static GraphPane *Instance()
	{
		static GraphPane _instance;
		return &_instance;
	}

protected:
	GraphPane(); // Prevent construction
	GraphPane(const GraphPane&) {}; // Prevent construction by copying
	GraphPane& operator =(const GraphPane&) { return *this; }; // Prevent assignment
	~GraphPane(); // Prevent unwanted destruction};
};
