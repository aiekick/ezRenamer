#include "baseLibrary.h"
#include <ezlibs/ezLog.hpp>
#include <imguipack/ImGuiPack.h>

#define INPUT_NODES_CATEGORY_NAME "Inputs"
#define OUTPUT_NODES_CATEGORY_NAME "Outputs"

//////////////////////////////////////////////////////////////////////////////////////////////
//// INIT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void BaseLibrary::clear() {
    m_categoryName.clear();
    m_subCategories.clear();
}

bool BaseLibrary::empty() const {
    return emptyCategory() && emptySubCategories() && emptyEntries();
}

bool BaseLibrary::emptyCategory() const {
    return m_categoryName.empty();
}

bool BaseLibrary::emptySubCategories() const {
    return m_subCategories.empty();
}

bool BaseLibrary::emptyEntries() const {
    return m_entries.empty();
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
            if (word == vec.front() && // first category of the path
                (word == INPUT_NODES_CATEGORY_NAME ||    // Category of Inputs node to show on top of the popup
                 word == OUTPUT_NODES_CATEGORY_NAME)) {  // Category of Outputs node to show on top of the popup
                if (pCat->m_mainSubCategories.find(word) != pCat->m_mainSubCategories.end()) {
                    pCat = &pCat->m_mainSubCategories.at(word);
                } else {
                    pCat = pCat->m_addCategory(word, pCat->m_mainSubCategories);
                }
            } else {
                if (pCat->m_subCategories.find(word) != pCat->m_subCategories.end()) {
                    pCat = &pCat->m_subCategories.at(word);
                } else {
                    pCat = pCat->m_addCategory(word, pCat->m_subCategories);
                }
            }
        }
        pCat->m_entries[vLibraryEntry.nodeLabel] = vLibraryEntry;
    }
}

bool BaseLibrary::showMenu(LibraryEntry& vOutEntry) {
    return m_showContent(vOutEntry, 0);
}

bool BaseLibrary::filterNodesForSomeInputSlotTypes(const SlotTypes& vInputSlotTypes) {
    return m_filterNodesForSomeInputSlotTypes(vInputSlotTypes, 0);
}

BaseNodeWeak BaseLibrary::createChildNodeInGraph(const LibraryEntry& vEntry, const BaseGraphWeak& vGraph) {
    assert(vEntry.createNodeFunctor != nullptr);
    if (vEntry.createNodeFunctor != nullptr) {
        return vEntry.createNodeFunctor(vGraph);
    }
    return {};
}

BaseLibrary* BaseLibrary::m_addCategory(const CategoryName& vCategoryName, CategoriesCnt& vCategories) {
    vCategories[vCategoryName].m_categoryName = vCategoryName;
    return &vCategories.at(vCategoryName);
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
    if (vLevel == 0) {
        for (auto& category : m_mainSubCategories) {
            ret |= category.second.m_showMenu(vOutEntry, vLevel + 1);
        }
        ImGui::Separator();
    }
    for (auto& category : m_subCategories) {
        ret |= category.second.m_showMenu(vOutEntry, vLevel + 1);
    }
    if (!m_entries.empty() && !m_subCategories.empty()) {
        ImGui::Separator();
    }
    ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);
    for (auto& item : m_entries) {
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
                if (category.second.emptyEntries()) {  // no entries, we will remove the category
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
            for (auto& item : m_entries) {
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
                m_entries.erase(node_label);
            }
        }        
    }
    return ret;
}
