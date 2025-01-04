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

bool BaseLibrary::empty() const {
    return m_categoryName.empty() && m_subCategories.empty() && m_libraryItems.empty();
}

bool BaseLibrary::emptyLeafs() const {
    return m_libraryItems.empty();
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

bool BaseLibrary::filterNodesForSomeInputSlotTypes(const SlotTypes& vInputSlotTypes) {
    return m_filterNodesForSomeInputSlotTypes(vInputSlotTypes, 0);
}

BaseLibrary* BaseLibrary::m_addCategory(const CategoryName& vCategoryName) {
    m_subCategories[vCategoryName].m_categoryName = vCategoryName;
    return &m_subCategories.at(vCategoryName);
}

bool BaseLibrary::m_showMenu(LibraryEntry& vOutEntry, int32_t vLevel) {
    bool ret = false;
    ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);
    if (ImGui::BeginMenu(m_categoryName.c_str())) {
        ret = m_showContent(vOutEntry, vLevel + 1);
        ImGui::EndMenu();
    }
    return ret;
}

bool BaseLibrary::m_showContent(LibraryEntry& vOutEntry, int32_t vLevel) {
    bool ret = false;
    for (auto& category : m_subCategories) {
        ret |= category.second.m_showMenu(vOutEntry, vLevel + 1);
    }
    if (!m_libraryItems.empty() && !m_subCategories.empty()) {
        ImGui::Separator();
    }
    ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);
    for (auto& item : m_libraryItems) {
        if (ImGui::MenuItem(item.first.c_str())) {
            vOutEntry = item.second;
            ret = true;
            // no break for not show a glitch when the others
            // menus will not been displayed after click this frame,
            // but all the next frame
        }
    }
    return ret;
}

/*
will remove all nodes from library who not have 
at least a slot of the wanted type
*/
bool BaseLibrary::m_filterNodesForSomeInputSlotTypes(const SlotTypes& vInputSlotTypes, int32_t vLevel) {
    bool ret = false;
    if (!vInputSlotTypes.empty()) {
        // childs filtering
        {
            std::set<CategoryName> categoryToRemove;
            for (auto& category : m_subCategories) {
                ret |= category.second.m_filterNodesForSomeInputSlotTypes(vInputSlotTypes, vLevel + 1);
                if (category.second.emptyLeafs()) {  // no entry leafs, we will remove the category
                    categoryToRemove.emplace(category.first);
                }
            }
            // now remove empty category entry
            for (const auto& category_name : categoryToRemove) {
                m_subCategories.erase(category_name);
            }
        }

        // leafs
        {
            bool found = false;
            std::set<NodeLabel> nodeToRemove;
            for (auto& item : m_libraryItems) {
                found = false;
                for (const auto& type : vInputSlotTypes) {
                    if (item.second.inputSlotTypes.find(type) != item.second.inputSlotTypes.end()) {
                        found = true;  // we will keep this node
                        ret = true;
                        break;
                    }
                }
                // entry to remove, since no input slots of the wanted type was found
                if (!found) {
                    nodeToRemove.emplace(item.first);
                }
            }
            // now remove unwanted entry
            for (const auto& node_label : nodeToRemove) {
                m_libraryItems.erase(node_label);
            }
        }        
    }
    return ret;
}
