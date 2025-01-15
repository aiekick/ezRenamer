#include "baseLibrary.h"
#include <ezlibs/ezLog.hpp>
#include <imguipack/ImGuiPack.h>

#define INPUT_NODES_CATEGORY_NAME "Inputs"
#define OUTPUT_NODES_CATEGORY_NAME "Outputs"
#define TOP_LEVEL_CATEGORY_LESS_NODES_CATEGORY_NAME "TopLevelCategoryLess"

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
    if (vLibraryEntry.nodeType.empty()) {
        LogVarError("Cant add entry cause nodeType is empty");
        return;
    }
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
            if (word == vec.front() &&                   // first category of the path
                (word == INPUT_NODES_CATEGORY_NAME ||    // Category of Inputs node to show on top of the popup
                 word == OUTPUT_NODES_CATEGORY_NAME)) {  // Category of Outputs node to show on top of the popup
                if (pCat->m_mainSubCategories.find(word) != pCat->m_mainSubCategories.end()) {
                    pCat = &pCat->m_mainSubCategories.at(word);
                } else {
                    pCat = pCat->m_addCategory(word, pCat->m_mainSubCategories);
                }
            } else if (word != vec.front() || word != TOP_LEVEL_CATEGORY_LESS_NODES_CATEGORY_NAME) {  // special cat, on top without category
                if (pCat->m_subCategories.find(word) != pCat->m_subCategories.end()) {
                    pCat = &pCat->m_subCategories.at(word);
                } else {
                    pCat = pCat->m_addCategory(word, pCat->m_subCategories);
                }
            }
        }
        if (vLibraryEntry.nodePath == TOP_LEVEL_CATEGORY_LESS_NODES_CATEGORY_NAME) {  // special cat, on top without category
            pCat->m_mainEntries[vLibraryEntry.nodeType] = vLibraryEntry;
        } else {
            pCat->m_entries[vLibraryEntry.nodeType] = vLibraryEntry;
        }
    }
}

bool BaseLibrary::showMenu(LibraryEntry& vOutEntry) {
    return m_showContent(vOutEntry, 0);
}

bool BaseLibrary::filterNodesForSomeInputSlotTypes(const SlotTypes& vInputSlotTypes) {
    return m_filterNodesForSomeInputSlotTypes(vInputSlotTypes, 0);
}

BaseNodeWeak BaseLibrary::createChildNodeInGraph(const NodeType& vNodeType, const BaseGraphWeak& vGraph) {
    LibraryEntry entry;
    if (m_findEntryFromNodeType(vNodeType, entry)) {
        return createChildNodeInGraph(entry, vGraph);
    }
    return {};
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
        ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);
        for (auto& item : m_mainEntries) {
            if (ImGui::MenuItem(item.second.nodeLabel.c_str())) {
                vOutEntry = item.second;
                ret = true;
                // no break for not show a glitch when the others
                // menus will not been displayed after click this frame,
                // but all the next frame
            }
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
        if (ImGui::MenuItem(item.second.nodeLabel.c_str())) {
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
        if (vLevel == 0) {  // main childs filtering
            ret |= m_filterCategories(vInputSlotTypes, m_mainSubCategories, 0);
        }

        {  // childs filtering
            ret |= m_filterCategories(vInputSlotTypes, m_subCategories, vLevel);
        }

        if (vLevel == 0) {  // main entries
            ret |= m_filterEntries(vInputSlotTypes, m_mainEntries);
        }

        {  // entries
            ret |= m_filterEntries(vInputSlotTypes, m_entries);
        }
    }
    return ret;
}

bool BaseLibrary::m_filterCategories(const SlotTypes& vInputSlotTypes, CategoriesCnt& vCategories, int32_t vLevel) {
    bool ret = false;
    std::set<CategoryName> categoryToRemove;
    for (auto& category : vCategories) {
        ret |= category.second.m_filterNodesForSomeInputSlotTypes(vInputSlotTypes, vLevel + 1);
        if (category.second.emptyEntries()) {  // no entries, we will remove the category
            categoryToRemove.emplace(category.first);
        }
    }
    // now remove empty category entry
    for (const auto& category_name : categoryToRemove) {
        vCategories.erase(category_name);
    }
    return ret;
}

bool BaseLibrary::m_filterEntries(const SlotTypes& vInputSlotTypes, LibraryEntriesCnt& vEntries) {
    bool ret = false;
    bool found = false;
    std::set<NodeLabel> nodeToRemove;
    for (auto& item : vEntries) {
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
        vEntries.erase(node_label);
    }
    return ret;
}

bool BaseLibrary::m_findEntryFromNodeType(const NodeType& vNodeType, BaseLibrary::LibraryEntry& vOutEntry) const {
    if (m_mainEntries.find(vNodeType) != m_mainEntries.end()) {
        vOutEntry = m_mainEntries.at(vNodeType);
        return true;
    } else if (m_entries.find(vNodeType) != m_entries.end()) {
        vOutEntry = m_entries.at(vNodeType);
        return true;
    } else {
        for (const auto& cat : m_mainSubCategories) {
            if (cat.second.m_findEntryFromNodeType(vNodeType, vOutEntry)) {
                return true;
            }
        }  
        for (const auto& cat : m_subCategories) {
            if (cat.second.m_findEntryFromNodeType(vNodeType, vOutEntry)) {
                return true;
            }
        }        
    }
    return false;
}
