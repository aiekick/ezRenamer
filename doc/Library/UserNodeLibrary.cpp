/*
Copyright 2022-2023 Stephane Cuillerdier (aka aiekick)

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

#include "UserNodeLibrary.h"

#include <imgui.h>
#include <LumoBackend/Graph/Base/BaseNode.h>
#include <ctools/FileHelper.h>

#include <Graph/Manager/NodeManager.h>

#include <stdio.h>
#include <string.h>

#include <functional>

#include <Plugins/PluginManager.h>

#include <Graph/Factory/NodeFactory.h>

// for directory/files lister
#if defined(__WIN32__) || defined(_WIN32)
#include <dirent/dirent.h>
#define PATH_SEP '\\'
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
#include <dirent.h>
#define PATH_SEP '/'
#endif

#define LIBRARY_EFECT_ROOT_PATH "NodeLibrary"

//////////////////////////////////////////////////////////////////////////////////////////////
//// CUSTOM MENU /////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void UserNodeLibrary::AnalyseRootDirectory() {
    // internals

    // Assets
    m_RootLibraryCategory.AddCustom("Assets/Load", "3D Model", "MESH");
    m_RootLibraryCategory.AddCustom("Assets/Load", "2D Texture", "TEXTURE_2D");
    m_RootLibraryCategory.AddCustom("Assets/Load", "2D CubeMap", "CUBE_MAP");

    // Exporters
    m_RootLibraryCategory.AddCustom("Assets/Save", "3D Model", "MODEL_EXPORTER");
    m_RootLibraryCategory.AddCustom("Assets/Save", "2D Texture", "TEXTURE_2D_EXPORTER");

    // Misc
    m_RootLibraryCategory.AddCustom("", "Scene Merger", "SCENE_MERGER");
    m_RootLibraryCategory.AddCustom("Scene", "Grid / Axis", "GRID_AXIS");

    // Renderers
    m_RootLibraryCategory.AddCustom("Renderers", "Matcap", "MATCAP_RENDERER");
    m_RootLibraryCategory.AddCustom("Renderers", "Model", "MODEL_RENDERER");

    // Variables
    m_RootLibraryCategory.AddCustom("Widgets", "Boolean", "WIDGET_BOOLEAN");
    m_RootLibraryCategory.AddCustom("Widgets", "Color", "WIDGET_COLOR");

    // Utils
    m_RootLibraryCategory.AddCustom("Utils", "Math", "MATH");
    m_RootLibraryCategory.AddCustom("Model", "Model Buffers", "MESH_BUFFERS");

    // shader nodes
    AnalyseRootDirectoryRecurs(LIBRARY_EFECT_ROOT_PATH, 0, &m_RootLibraryCategory);

    // plugin nodes
    auto pluginEntrys = PluginManager::Instance()->GetLibraryEntrys();
    for (auto entry : pluginEntrys) {
        m_RootLibraryCategory.AddLibraryEntry(entry);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// INIT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

UserNodeLibrary::UserNodeLibrary() = default;

//////////////////////////////////////////////////////////////////////////////////////////////
//// MENU ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

BaseNodeWeak UserNodeLibrary::ShowNewNodeMenu(BaseNodeWeak vNodeGraph, BaseNodeState *vBaseNodeState) {
    if (vNodeGraph.expired())
        return BaseNodeWeak();

    BaseNodeWeak createdNode;

    ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);

    if (ImGui::BeginPopup("CreateNewNode")) {
        auto entry = m_RootLibraryCategory.ShowContent(vNodeGraph, vBaseNodeState, 0);
        if (!entry.first.empty()) {
            createdNode = CreateNode(vNodeGraph, entry);
            auto createdNodePtr = createdNode.lock();
            if (createdNodePtr) {
                createdNodePtr->m_RootNode = vNodeGraph;
                createdNodePtr->TreatNotification(GraphIsLoaded);
            }
        }

        ImGui::EndPopup();
    } else {
        auto graphPtr = vNodeGraph.lock();
        if (graphPtr) {
            graphPtr->m_CreateNewNode = false;
        }
    }

    if (!createdNode.expired()) {
        auto nodePtr = createdNode.lock();
        auto graphPtr = vNodeGraph.lock();
        if (nodePtr && graphPtr) {
            nd::SetNodePosition(nodePtr->nodeID, graphPtr->m_OpenPopupPosition);
        }
    }

    return createdNode;
}

BaseNodeWeak UserNodeLibrary::CreateNode(BaseNodeWeak vNodeGraph, const LibraryEntry &vLibraryEntry) {
    if (vNodeGraph.expired())
        return BaseNodeWeak();
    if (vLibraryEntry.first.empty())
        return BaseNodeWeak();

    BaseNodePtr nodePtr = nullptr;

    if (vLibraryEntry.first == "internal") {
        if (vLibraryEntry.second.type == LibraryItem::LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_INTERNAL) {
            nodePtr = NodeFactory::CreateNode(vNodeGraph, vLibraryEntry.second.nodeType);
        }
    } else if (vLibraryEntry.first == "plugins") {
        if (vLibraryEntry.second.type == LibraryItem::LibraryItemTypeEnum::LIBRARY_ITEM_TYPE_PLUGIN) {
            nodePtr = PluginManager::Instance()->CreatePluginNode(vLibraryEntry.second.nodeType);
        }
    }

    if (nodePtr) {
        auto graphPtr = vNodeGraph.lock();
        if (graphPtr) {
            /*if (vLibraryEntry.second.nodeType == "OUTPUT_3D")
            {
                graphPtr->m_Output3DNode = nodePtr;
            }
            else if (vLibraryEntry.second.nodeType == "OUTPUT_2D")
            {
                graphPtr->m_Output2DNode = nodePtr;
            }*/

            graphPtr->AddChildNode(nodePtr);

            return nodePtr;
        }
    }

    return BaseNodeWeak();
}

void UserNodeLibrary::AnalyseRootDirectoryRecurs(const char *vRootPath, int vIndent, LibraryCategory *vLibraryCategory) {
    if (!vLibraryCategory)
        return;

    DIR *dir = opendir(vRootPath);
    struct dirent *entry;

    if (!dir)
        return;

    while ((entry = readdir(dir)) != nullptr) {
        char path[1024];

        if (entry->d_type == DT_DIR)  // Directory
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", vRootPath, entry->d_name);

            printf("%*s[%s]\n", vIndent, "", entry->d_name);

            auto cat = vLibraryCategory->AddCategory(entry->d_name);

            AnalyseRootDirectoryRecurs(path, vIndent + 2, cat);
        } else if (entry->d_type == DT_REG)  // File
        {
            snprintf(path, sizeof(path), "%s/%s", vRootPath, entry->d_name);

            vLibraryCategory->AddShader(entry->d_name, path);

            printf("%*s- %s\n", vIndent, "", entry->d_name);
        }
    }

    closedir(dir);
}