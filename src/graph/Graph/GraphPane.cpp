// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include "GraphPane.h"

#include <Gui/MainFrame.h>

#include <Panes/Manager/LayoutManager.h>
#include <Gui/ImWidgets.h>

#include <Project/ProjectFile.h>
#include <ctools/FileHelper.h>


#include <imgui/imgui_internal.h>

#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <Graph/Layout/GraphLayout.h>

#include <ctools/cTools.h>
#include <ctools/FileHelper.h>

#include <Graph/Library/UserNodeLibrary.h>

#include <cinttypes> // printf zu

static int MainPane_WidgetId = 0;

GraphPane::GraphPane() = default;
GraphPane::~GraphPane() = default;

bool GraphPane::Init()
{
	m_RootNode = std::make_shared<BaseNode>();
	m_RootNode->m_This = m_RootNode;
	m_RootNode->Init();
	UserNodeLibrary::Instance()->AnalyseRootDirectory();

	// add graph pane
	BaseNode::sOpenGraphCallback = std::bind(&GraphPane::AddGraphPane, this, std::placeholders::_1);

	return true;
}

void GraphPane::Unit()
{
	m_RootNode.reset();
}

void GraphPane::Clear()
{
	/*if (!vNode.expired())
	{
		auto ptr = vNode.lock();
		if (ptr)
		{

		}
	}*/

	ClearGraphPanes();
	m_RootNode->ClearGraph();
}

void GraphPane::SelectNode(ct::cWeak<BaseNode> vNode)
{

}

void GraphPane::DrawDebugInfos()
{
	if (!m_LastFocusedGraph.expired())
	{
		auto graphPtr = m_LastFocusedGraph.lock();
		if (graphPtr.use_count())
		{
			if (!graphPtr->m_BaseNodeState.current_selected_node.expired())
			{
				auto nodePtr = graphPtr->m_BaseNodeState.current_selected_node.lock();
				if (nodePtr.use_count())
				{
					nodePtr->DrawDebugInfos(&graphPtr->m_BaseNodeState);
				}
			}
			else
			{
				graphPtr->DrawDebugInfos(&graphPtr->m_BaseNodeState);
			}
		}
	}
}

void GraphPane::DrawProperties()
{
	if (!m_LastFocusedGraph.expired())
	{
		auto graphPtr = m_LastFocusedGraph.lock();
		if (graphPtr.use_count())
		{
			if (!graphPtr->m_BaseNodeState.current_selected_node.expired())
			{
				auto nodePtr = graphPtr->m_BaseNodeState.current_selected_node.lock();
				if (nodePtr.use_count())
				{
					nodePtr->DrawProperties(&graphPtr->m_BaseNodeState);
				}
			}
			else
			{
				graphPtr->DrawProperties(&graphPtr->m_BaseNodeState);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
//// IMGUI PANE ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

int GraphPane::DrawPanes(int vWidgetId, std::string vUserDatas)
{
	MainPane_WidgetId = vWidgetId;

	ImGui::SetPUSHID(MainPane_WidgetId);

	bool change = false;

	if (LayoutManager::Instance()->m_Pane_Shown & m_PaneFlag)
	{
		// main graph
		bool opened = true;
		DrawGraph(m_RootNode, opened, true, 0);

		// childs graph
		size_t countPanes = m_GraphPanes.size();
		for (auto nodeEntry : m_GraphPanes)
		{
			DrawGraph(nodeEntry.first, nodeEntry.second, false, countPanes);
		}
	}

	if (change)
	{
		//ProjectFile::Instance()->SetProjectChange();
	}

	return MainPane_WidgetId;
}

void GraphPane::DrawDialogsAndPopups(std::string vUserDatas)
{
	//ImVec2 min = MainFrame::Instance()->m_DisplaySize * 0.5f;
	//ImVec2 max = MainFrame::Instance()->m_DisplaySize;

	/*if (ImGuiFileDialog::Instance()->Display("ShaderDlg",
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking, min, max))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			auto GoodFilePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			//auto UserDatas = std::string((const char*)ImGuiFileDialog::Instance()->GetUserDatas());

			if (FileHelper::Instance()->IsFileExist(GoodFilePathName))
			{
				MainFrame::Instance()->OpenShaderFile(GoodFilePathName);
			}
		}

		ImGuiFileDialog::Instance()->Close();
	}*/
}

int GraphPane::DrawWidgets(int vWidgetId, std::string vUserDatas)
{
	return vWidgetId;
}

void GraphPane::DeleteNodesIfAnys()
{
	if (m_RootNode.use_count())
		m_RootNode->DestroyNodesIfAnys();
}

void GraphPane::AddGraphPane(std::weak_ptr<BaseNode> vNodeGraphToShow)
{
	if (!vNodeGraphToShow.expired())
	{
		auto nodePtr = vNodeGraphToShow.lock();
		if (nodePtr && !nodePtr->name.empty())
		{
			bool found = false;
			for (auto paneIt = m_GraphPanes.begin(); paneIt != m_GraphPanes.end(); paneIt++)
			{
				if (!paneIt->first.expired())
				{
					auto panePtr = paneIt->first.lock();
					if (panePtr)
					{
						if (panePtr == nodePtr)
							found = true;
					}
				}
			}

			if (!found)
			{
				m_GraphPanes.emplace_back(vNodeGraphToShow, true);
				nodePtr->InitGraph();
				nodePtr->uniquePaneId = nodePtr->name + "##" + ct::toStr((int)nodePtr->nodeID);
				LayoutManager::Instance()->AddSpecificPaneToExisting(nodePtr->uniquePaneId.c_str(), m_PaneName);
			}

			LayoutManager::Instance()->FocusSpecificPane(nodePtr->uniquePaneId.c_str());
		}
	}
}

void GraphPane::RemoveGraphPane(std::weak_ptr<BaseNode> vNodeGraphToShow)
{
	if (!vNodeGraphToShow.expired())
	{
		auto nodePtr = vNodeGraphToShow.lock();
		if (nodePtr && !nodePtr->name.empty())
		{
			for (auto paneIt = m_GraphPanes.begin(); paneIt != m_GraphPanes.end(); paneIt++)
			{
				if (!paneIt->first.expired())
				{
					auto panePtr = paneIt->first.lock();
					if (panePtr)
					{
						if (panePtr == nodePtr)
						{
							m_GraphPanes.erase(paneIt);
							return;
						}
					}
				}
			}
		}
	}
}

void GraphPane::ClearGraphPanes()
{
	m_GraphPanes.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// LOAD / SAVE GRAPH ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void GraphPane::FinalizeGraphLoading()
{
	m_RootNode->FinalizeGraphLoading();
}

std::string GraphPane::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	std::string res;

	res += m_RootNode->getXml(vOffset, vUserDatas);

	return res;
}

bool GraphPane::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
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

	m_RootNode->setFromXml(vElem, vParent, vUserDatas);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////
//// PRIVATE //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool GraphPane::DrawGraph(std::weak_ptr<BaseNode> vNode, bool &vCanShow, bool vRootNode, size_t vInitialPanesCount)
{
	if (vCanShow)
	{
		if (!vNode.expired())
		{
			auto nodeEntryPtr = vNode.lock();
			if (nodeEntryPtr)
			{
				if (!nodeEntryPtr->uniquePaneId.empty() || vRootNode)
				{
					if (LayoutManager::Instance()->m_Pane_Shown & m_PaneFlag)
					{
						if (vRootNode)
						{
							if (ImGui::BeginFlag<PaneFlags>(m_PaneName,
								&LayoutManager::Instance()->m_Pane_Shown, m_PaneFlag,
								//ImGuiWindowFlags_NoTitleBar |
								ImGuiWindowFlags_MenuBar |
								//ImGuiWindowFlags_NoMove |
								ImGuiWindowFlags_NoCollapse |
								//ImGuiWindowFlags_NoResize |
								ImGuiWindowFlags_NoScrollbar |
								ImGuiWindowFlags_NoBringToFrontOnFocus))
							{

								auto win = ImGui::GetCurrentWindowRead();
								if (win)
								{
									if (win->DockTabIsVisible)
									{
										m_LastFocusedGraph = m_RootNode;

										if (ImGui::BeginMenuBar())
										{
											if (ImGui::MenuItem("Layout", "apply Layout"))
											{
												GraphLayout::Instance()->ApplyLayout(m_RootNode);
												//ProjectFile::Instance()->SetProjectChange();
											}

											m_RootNode->DrawToolMenu();

											/*if (m_RootNode->m_NodeGraphContext)
											{
												SetCurrentEditor(m_RootNode->m_NodeGraphContext);
												if (GetSelectedObjectCount())
												{
													if (ImGui::BeginMenu("Selection"))
													{
														if (ImGui::MenuItem("Zoom on Selection"))
														{
															m_RootNode->ZoomToSelection();
														}

														if (ImGui::MenuItem("Center on Selection"))
														{
															m_RootNode->NavigateToSelection();
														}

														ImGui::EndMenu();
													}
												}

												if (ImGui::BeginMenu("Content"))
												{
													if (ImGui::MenuItem("Zoom on Content"))
													{
														m_RootNode->ZoomToContent();
													}

													if (ImGui::MenuItem("Center on Content"))
													{
														m_RootNode->NavigateToContent();
													}

													ImGui::EndMenu();
												}
											}*/

											if (ImGui::MenuItem("Clear"))
											{
												ClearGraphPanes();
												//ProjectFile::Instance()->SetProjectChange();
											}

											if (ImGui::BeginMenu("Style"))
											{
												m_RootNode->DrawStyleMenu();
												GraphLayout::Instance()->DrawSettings();

												ImGui::EndMenu();
											}

											ImGui::EndMenuBar();
										}

										m_RootNode->DrawGraph();
									}
								}
							}
						}
						else
						{
							if (ImGui::Begin(nodeEntryPtr->uniquePaneId.c_str(), &vCanShow,
								//ImGuiWindowFlags_NoTitleBar |
								ImGuiWindowFlags_MenuBar |
								//ImGuiWindowFlags_NoMove |
								ImGuiWindowFlags_NoCollapse |
								//ImGuiWindowFlags_NoResize |
								ImGuiWindowFlags_NoScrollbar |
								ImGuiWindowFlags_NoBringToFrontOnFocus))
							{
								auto win = ImGui::GetCurrentWindowRead();
								if (win)
								{
									if (win->DockTabIsVisible)
									{
										m_LastFocusedGraph = nodeEntryPtr;
										
										if (ImGui::BeginMenuBar())
										{
											if (ImGui::MenuItem("Layout", "apply Layout"))
											{
												GraphLayout::Instance()->ApplyLayout(nodeEntryPtr);
												//ProjectFile::Instance()->SetProjectChange();
											}

											nodeEntryPtr->DrawToolMenu();

											/*if (nodeEntryPtr->m_NodeGraphContext)
											{
												SetCurrentEditor(nodeEntryPtr->m_NodeGraphContext);
												if (GetSelectedObjectCount())
												{
													if (ImGui::BeginMenu("Selection"))
													{
														if (ImGui::MenuItem("Zoom on Selection"))
														{
															nodeEntryPtr->ZoomToSelection();
														}

														if (ImGui::MenuItem("Center on Selection"))
														{
															nodeEntryPtr->NavigateToSelection();
														}

														ImGui::EndMenu();
													}
												}

												if (ImGui::BeginMenu("Content"))
												{
													if (ImGui::MenuItem("Zoom on Content"))
													{
														nodeEntryPtr->ZoomToContent();
													}

													if (ImGui::MenuItem("Center on Content"))
													{
														nodeEntryPtr->NavigateToContent();
													}

													ImGui::EndMenu();
												}
											}*/

											if (ImGui::BeginMenu("Style"))
											{
												nodeEntryPtr->DrawStyleMenu();
												GraphLayout::Instance()->DrawSettings();
												ImGui::EndMenu();
											}

											ImGui::EndMenuBar();
										}

										nodeEntryPtr->DrawGraph();

										// si on a cree un autre pane, pour eviter de blocker 
										// l'iterateur on quitte pour cette frame
										if (vInitialPanesCount != m_GraphPanes.size())
										{
											ImGui::End();
											return true;
										}
									}
								}
							}
						}

						ImGui::End();

						if (!vCanShow) // visibility state changed => we remove the graph
						{
							RemoveGraphPane(nodeEntryPtr);
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}