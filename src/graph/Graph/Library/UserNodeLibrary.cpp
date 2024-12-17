// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "UserNodeLibrary.h"

#include <imgui/imgui.h>
#include <Graph/Base/BaseNode.h>
#include <ctools/FileHelper.h>

#include <stdio.h>
#include <string.h>

// for directory/files lister
#if defined(__WIN32__) || defined(_WIN32)
#include <ImGuiFileDialog/dirent/dirent.h>
#define PATH_SEP '\\'
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
#include <dirent.h>
#define PATH_SEP '/'
#endif

#define NODE_LIBRARY_EFECT_ROOT_PATH "NodeLibrary"

//////////////////////////////////////////////////////////////////////////////////////////////
//// INIT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

LibraryCategory::LibraryCategory()
{
	
}

LibraryCategory::~LibraryCategory()
{
	
}

void LibraryCategory::Clear()
{
	m_CategoryName.clear();
	m_LibraryItems.clear();
	m_SubCategories.clear();
}

LibraryCategory* LibraryCategory::AddCategory(const std::string& vCategoryName)
{
	if (vCategoryName.empty()) return 0;

	m_SubCategories[vCategoryName].m_CategoryName = vCategoryName;

	return &m_SubCategories[vCategoryName];
}

void LibraryCategory::AddShader(const std::string& vShaderName, const std::string& vShaderPath)
{
	if (vShaderName.empty()) return;
	if (vShaderPath.empty()) return;

	auto ps = FileHelper::Instance()->ParsePathFileName(vShaderName);
	if (ps.isOk)
	{
		LibraryEntry entry;

		if (ps.ext == "comp") entry.first = "compute";
		if (ps.ext == "frag") entry.first = "fragment";
		if (ps.ext == "scen") entry.first = "scene";

		if (!entry.first.empty())
		{
			entry.second.type = LibraryItem::LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_SHADER;
			entry.second.shaderpath = vShaderPath;
			m_LibraryItems[ps.name] = entry;
		}
	}
}

void LibraryCategory::AddCustom(const std::string& vCategoryPath, const std::string& vNodeLabel, const std::string& vNodeType)
{
	if (vCategoryPath.empty()) return;
	if (vNodeLabel.empty()) return;

	auto vec = ct::splitStringToVector(vCategoryPath, '/');
	if (!vec.empty())
	{
		auto cat = this;
		for (auto word : vec)
		{
			if (cat->m_SubCategories.find(word) != cat->m_SubCategories.end())
			{
				cat = &cat->m_SubCategories[word];
			}
			else
			{
				cat = cat->AddCategory(word);
			}
		}

		LibraryEntry entry;
		entry.second.type = LibraryItem::LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_INTERNAL;
		entry.first = "internal";
		entry.second.nodeLabel = vNodeLabel;
		entry.second.nodeType = vNodeType;
		cat->m_LibraryItems[vNodeLabel] = entry;
	}
}

LibraryEntry LibraryCategory::ShowMenu(std::weak_ptr<BaseNode> vNodeGraph, BaseNodeStateStruct *vCanvasState, int vLevel)
{
	LibraryEntry entry;

	if (ImGui::BeginMenu(m_CategoryName.c_str()))
	{
		auto ent = ShowContent(vNodeGraph, vCanvasState, vLevel + 1);
		if (!ent.first.empty())
		{
			entry = ent;
		}

		ImGui::EndMenu();
	}

	return entry;
}

LibraryEntry LibraryCategory::ShowContent(std::weak_ptr<BaseNode> vNodeGraph, BaseNodeStateStruct *vCanvasState, int vLevel)
{
	LibraryEntry entry;
	
	if (vCanvasState && vLevel == 0)
	{
		if (!vCanvasState->linkFromSlot.expired())
		{
			if (ImGui::MenuItem("extract"))
			{
				auto slotPtr = vCanvasState->linkFromSlot.lock();
				if (slotPtr)
				{
					entry.second.type = LibraryItem::LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_INTERNAL;
					entry.first = "internal";
					entry.second.nodeLabel = "extract";
					entry.second.nodeType = "extract";
					entry.second.nodeSlot = *slotPtr;
				}
			}

			ImGui::Separator();
		}
	}

	for (auto &category : m_SubCategories)
	{
		auto ent = category.second.ShowMenu(vNodeGraph, vCanvasState, vLevel + 1);
		if (!ent.first.empty())
		{
			entry = ent;
		}
	}

	if (!m_LibraryItems.empty() &&
		!m_SubCategories.empty())
	{
		ImGui::Separator();
	}

	for (auto &item : m_LibraryItems)
	{
		if (ImGui::MenuItem(item.first.c_str()))
		{
			entry = item.second;

			break;
		}
	}

	return entry;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// INIT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

UserNodeLibrary::UserNodeLibrary()
{
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// MENU ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

std::weak_ptr<BaseNode> UserNodeLibrary::ShowNewNodeMenu(std::weak_ptr<BaseNode> vNodeGraph, BaseNodeStateStruct *vCanvasState)
{
	if (vNodeGraph.expired()) return std::weak_ptr<BaseNode>();
	
	std::weak_ptr<BaseNode> createdNode;

	if (ImGui::BeginPopup("CreateNewNode"))
	{
		auto entry = m_RootLibraryCategory.ShowContent(vNodeGraph, vCanvasState, 0);
		if (!entry.first.empty())
		{
			createdNode = CreateNode(vNodeGraph, entry);
		}

		ImGui::EndPopup();
	}
	else
	{
		auto graphPtr = vNodeGraph.lock();
		if (graphPtr)
		{
			graphPtr->m_CreateNewNode = false;
		}
	}

	if (!createdNode.expired())
	{
		auto nodePtr = createdNode.lock();
		auto graphPtr = vNodeGraph.lock();
		if (nodePtr && graphPtr)
		{
			//SetNodePosition(nodePtr->nodeID, graphPtr->m_OpenPopupPosition);

			/*
			if (auto startSlot = m_NewNodeLinkSlot)
			{
				auto& pins = startSlot->Kind == SlotKiInput ? node->Outputs : node->Inputs;

				for (auto& pin : pins)
				{
					if (CanCreateLink(startSlot, &pin))
					{
						auto endSlot = &pin;
						if (startSlot->Kind == SlotKiInput)
							startSlot.swap(endSlot);

						s_Links.emplace_back(Link(GetNextId(), startSlot->ID, endSlot->ID));
						s_Links.back().Color = GetIconColor(startSlot->Type);

						break;
					}
				}
			}
			*/
		}
	}

	return createdNode;
}

std::weak_ptr<BaseNode> UserNodeLibrary::CreateNode(std::weak_ptr<BaseNode> vNodeGraph, const LibraryEntry& vLibraryEntry)
{
	if (vNodeGraph.expired()) return std::weak_ptr<BaseNode>();
	if (vLibraryEntry.first.empty()) return std::weak_ptr<BaseNode>();

	/*if (vLibraryEntry.first == "compute")
	{
		if (vLibraryEntry.second.type == LibraryItem::LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_SHADER)
		{
			auto shaderCode = FileHelper::Instance()->LoadFileToString(vLibraryEntry.second.shaderpath);
			if (!shaderCode.empty())
			{
				auto node = ComputeNode::Create(shaderCode);
				if (node)
				{
					auto graphPtr = vNodeGraph.lock();
					if (graphPtr)
					{
						graphPtr->AddChildNode(node);
						auto ps = FileHelper::Instance()->ParsePathFileName(vLibraryEntry.second.shaderpath);
						if (ps.isOk)
						{
							node->name = ps.name;
						}
						return node;
					}
				}
			}
			else
			{
				LogVarDebug("Can't load file %s", vLibraryEntry.second.shaderpath.c_str());
			}
		}
	}
	else if (vLibraryEntry.first == "fragment")
	{
		if (vLibraryEntry.second.type == LibraryItem::LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_SHADER)
		{
			auto shaderCode = FileHelper::Instance()->LoadFileToString(vLibraryEntry.second.shaderpath);
			if (!shaderCode.empty())
			{
				auto node = FragmentNode::Create(shaderCode);
				if (node)
				{
					auto graphPtr = vNodeGraph.lock();
					if (graphPtr)
					{
						graphPtr->AddChildNode(node);
						auto ps = FileHelper::Instance()->ParsePathFileName(vLibraryEntry.second.shaderpath);
						if (ps.isOk)
						{
							node->name = ps.name;
						}
						return node;
					}
				}
			}
			else
			{
				LogVarDebug("Can't load file %s", vLibraryEntry.second.shaderpath.c_str());
			}
		}
	}
	else */
	else if (vLibraryEntry.first == "internal")
	{
		if (vLibraryEntry.second.type == LibraryItem::LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_INTERNAL)
		{
			/*if (vLibraryEntry.second.nodeType == "SdfNode") 
				return CreateCustomNode<SdfNode>(vNodeGraph);
			else if (vLibraryEntry.second.nodeType == "MesherSliceNode") 
				return CreateCustomNode<MesherSliceNode>(vNodeGraph);
			else if (vLibraryEntry.second.nodeType == "MeshSimNode") 
				return CreateCustomNode<MeshSimNode>(vNodeGraph);
			else if (vLibraryEntry.second.nodeType == "SdfSlicerNode") 
				return CreateCustomNode<SdfSlicerNode>(vNodeGraph);
			else */
			if (vLibraryEntry.second.nodeType == "CustomFunction")
			{
				return CreateCustomNode<BaseNode>(vNodeGraph);
			}
		}
	}

	return std::weak_ptr<BaseNode>();
}

void UserNodeLibrary::AnalyseRootDirectory()
{
	AnalyseRootDirectoryRecurs(NODE_LIBRARY_EFECT_ROOT_PATH, 0, &m_RootLibraryCategory);

	m_RootLibraryCategory.AddCustom("Blueprints", "Function", "CustomFunction");

	//m_RootLibraryCategory.AddCustom("Mesher", "Sdf Renderer", "SdfNode");
	//m_RootLibraryCategory.AddCustom("Sim", "Mesh Sim", "MeshSimNode");
	//m_RootLibraryCategory.AddCustom("Mesher", "Slice Mesher", "MesherSliceNode");
	//m_RootLibraryCategory.AddCustom("Mesher", "Sdf Slicer", "SdfSlicerNode");
	//m_RootLibraryCategory.AddCustom("Mesh", "Mesh", "Mesh");
}

void UserNodeLibrary::AnalyseRootDirectoryRecurs(const char *vRootPath, int vIndent, LibraryCategory *vLibraryCategory)
{
	if (!vLibraryCategory) return;

	DIR *dir = opendir(vRootPath);
	struct dirent *entry;

	if (!dir)
		return;

	while ((entry = readdir(dir)) != nullptr) 
	{
		char path[1024];
		
		if (entry->d_type == DT_DIR) // Directory
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			snprintf(path, sizeof(path), "%s/%s", vRootPath, entry->d_name);

			printf("%*s[%s]\n", vIndent, "", entry->d_name);
			
			auto cat = vLibraryCategory->AddCategory(entry->d_name);

			AnalyseRootDirectoryRecurs(path, vIndent + 2, cat);
		}
		else if (entry->d_type == DT_REG) // File
		{
			snprintf(path, sizeof(path), "%s/%s", vRootPath, entry->d_name);
			
			vLibraryCategory->AddShader(entry->d_name, path);
			
			printf("%*s- %s\n", vIndent, "", entry->d_name);
		}
	}

	closedir(dir);
}