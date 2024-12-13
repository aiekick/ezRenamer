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

// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "projectFile.h"

#include <ezlibs/ezLog.hpp>
#include <ezlibs/ezFile.hpp>

#include <LayoutManager.h>

#include <systems/settingsDialog.h>

ProjectFile::ProjectFile() = default;

ProjectFile::ProjectFile(const std::string& vFilePathName) {
    m_ProjectFilePathName = ez::file::simplifyFilePath(vFilePathName);
    auto ps = ez::file::parsePathFileName(m_ProjectFilePathName);
    if (ps.isOk) {
        m_ProjectFileName = ps.name;
        m_ProjectFilePath = ps.path;
    }
}

ProjectFile::~ProjectFile() = default;

void ProjectFile::Clear() {
    m_ProjectFilePathName.clear();
    m_ProjectFileName.clear();
    m_ProjectFilePath.clear();
    m_IsLoaded = false;
    m_IsThereAnyChanges = false;
    Messaging::Instance()->Clear();
}

void ProjectFile::ClearDatas() {

}

void ProjectFile::New() {
    Clear();
    ClearDatas();
    m_IsLoaded = true;
    m_NeverSaved = true;
    SetProjectChange(true);
}

void ProjectFile::New(const std::string& vFilePathName) {
    Clear();
    ClearDatas();
    m_ProjectFilePathName = ez::file::simplifyFilePath(vFilePathName);
    auto ps = ez::file::parsePathFileName(m_ProjectFilePathName);
    if (ps.isOk) {
        m_ProjectFileName = ps.name;
        m_ProjectFilePath = ps.path;
    }
    m_IsLoaded = true;
    SetProjectChange(false);
}

bool ProjectFile::Load() {
    return LoadAs(m_ProjectFilePathName);
}

// ils wanted to not pass the adress for re open case
// elwse, the clear will set vFilePathName to empty because with re open, target m_ProjectFilePathName
bool ProjectFile::LoadAs(const std::string vFilePathName) {
    if (!vFilePathName.empty()) {
        Clear();
        std::string filePathName = ez::file::simplifyFilePath(vFilePathName);
        if (LoadConfigFile(filePathName, "project")) {
            m_ProjectFilePathName = ez::file::simplifyFilePath(vFilePathName);
            auto ps = ez::file::parsePathFileName(m_ProjectFilePathName);
            if (ps.isOk) {
                m_ProjectFileName = ps.name;
                m_ProjectFilePath = ps.path;
            }
            m_IsLoaded = true;
            SetProjectChange(false);
        } else {
            Clear();
            LogVarError("Error : the project file %s cant be loaded", filePathName.c_str());
        }
    }    
    return m_IsLoaded;
}

bool ProjectFile::Save() {
    if (m_NeverSaved) {
        return false;
    }
    return false;
}

bool ProjectFile::SaveAs(const std::string& vFilePathName) {
    std::string filePathName = ez::file::simplifyFilePath(vFilePathName);
    auto ps = ez::file::parsePathFileName(filePathName);
    if (ps.isOk) {
        m_ProjectFilePathName = ez::file::composePath(ps.path, ps.name, PROJECT_EXT_DOT_LESS);
        m_ProjectFilePath = ps.path;
        m_NeverSaved = false;
        return Save();
    }
    return false;
}

bool ProjectFile::IsProjectLoaded() const {
    return m_IsLoaded;
}

bool ProjectFile::IsProjectNeverSaved() const {
    return m_NeverSaved;
}

bool ProjectFile::IsThereAnyProjectChanges() const {
    return m_IsThereAnyChanges;
}

void ProjectFile::SetProjectChange(bool vChange) {
    m_IsThereAnyChanges = vChange;
    m_WasJustSaved = true;
    m_WasJustSavedFrameCounter = 2U;
}

void ProjectFile::NewFrame() {
    if (m_WasJustSavedFrameCounter) {
        --m_WasJustSavedFrameCounter;
    } else {
        m_WasJustSaved = false;
    }
}

bool ProjectFile::WasJustSaved() {
    return m_WasJustSaved;
}

std::string ProjectFile::GetProjectFilepathName() const {
    return m_ProjectFilePathName;
}

ez::xml::Nodes ProjectFile::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node node;
    node.setName("project");
    node.addChilds(LayoutManager::Instance()->getXmlNodes(vUserDatas));
    node.addChilds(SettingsDialog::Instance()->getXmlNodes(vUserDatas));
    return node.getChildren();
}

bool ProjectFile::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    const auto& strName = vNode.getName();
    //const auto& strValue = vNode.getContent();
    //const auto& strParentName = vParent.getName();
    if (strName == "config") {
        return true;
    } else if (strName == "project") {
        LayoutManager::Instance()->RecursParsingConfig(vNode, vParent, vUserDatas);
        SettingsDialog::Instance()->RecursParsingConfig(vNode, vParent, vUserDatas);
    }
    return true;
}
