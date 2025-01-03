#include "baseLibrary.h"
#include <ezlibs/ezLog.hpp>
#include <imguipack/ImGuiPack.h>

//////////////////////////////////////////////////////////////////////////////////////////////
//// INIT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void BaseLibrary::clear() {
    m_categoryName.clear();
    m_subCategories.clear();
}

void BaseLibrary::addLibraryEntry(const LibraryEntry& vLibraryEntry) {
    if (vLibraryEntry.nodeLabel.empty()) {
        LogVarError("Cant add entry cause nodeLabel is empty");
        return;
    }
    auto vec = ez::str::splitStringToVector(vLibraryEntry.nodePath, '/');
    if (!vec.empty()) {
        auto pCat = this;
        for (auto word : vec) {
            if (word.empty()) {
                word = "Empty";
                LogVarError("A item of the path [%s] is empty. replace by \"Empty\"", vLibraryEntry.nodePath.c_str());
            }
            if (pCat->m_subCategories.find(word) != pCat->m_subCategories.end()) {
                pCat = &pCat->m_subCategories.at(word);
            } else {
                pCat = pCat->m_addCategory(word);
            }
        }
        pCat->m_libraryItems[vLibraryEntry.nodeLabel] = vLibraryEntry;
    }
}

bool BaseLibrary::showMenu(LibraryEntry& vOutEntry) {
    return m_showContent(vOutEntry, 0);
}

BaseLibrary* BaseLibrary::m_addCategory(const CategoryName& vCategoryName) {
    m_subCategories[vCategoryName].m_categoryName = vCategoryName;
    return &m_subCategories.at(vCategoryName);
}

bool BaseLibrary::m_showMenu(LibraryEntry& vLibraryEntry, int32_t vLevel) {
    bool ret = false;
    ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);
    if (ImGui::BeginMenu(m_categoryName.c_str())) {
        ret = m_showContent(vLibraryEntry, vLevel + 1);
        ImGui::EndMenu();
    }
    return ret;
}

bool BaseLibrary::m_showContent(LibraryEntry& vLibraryEntry, int32_t vLevel) {
    bool ret = false;
    for (auto& category : m_subCategories) {
        ret |= category.second.m_showMenu(vLibraryEntry, vLevel + 1);
    }
    if (!m_libraryItems.empty() && !m_subCategories.empty()) {
        ImGui::Separator();
    }
    ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);
    for (auto& item : m_libraryItems) {
        if (ImGui::MenuItem(item.first.c_str())) {
            vLibraryEntry = item.second;
            ret = true;
            // no break for not show a glitch when the others 
            // menus will not been displayed after click this frame, 
            // but all the next frame
        }
    }
    return ret;
}