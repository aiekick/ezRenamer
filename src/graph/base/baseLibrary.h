#pragma once

#include <set>
#include <map>
#include <string>
#include <cstdint>
#include <functional>
#include <ezlibs/ezCnt.hpp>
#include <ezlibs/ezMath.hpp>
#include <graph/base/baseNode.h>
#include <graph/base/baseGraph.h>

class BaseLibrary {
public:
    typedef std::string NodeSource;
    typedef std::string NodeLabel;
    typedef std::string NodeType;
    typedef std::string NodePath;
    typedef ez::fvec4 NodeColor;
    typedef std::string CategoryName;
    typedef std::string SlotType;
    typedef std::set<SlotType> SlotTypes;
    typedef std::function<BaseNodeWeak(const BaseGraphWeak&)> CreateNodeFunctor;
    struct LibraryEntry {
        NodeSource nodeSource;
        NodePath nodePath;  // can be a path like cat0/cat1/node
        NodeLabel nodeLabel;
        NodeType nodeType;
        NodeColor nodeColor;
        SlotTypes inputSlotTypes;
        SlotTypes outputSlotTypes;
        CreateNodeFunctor createNodeFunctor;
        LibraryEntry() = default;
        LibraryEntry(
            const NodePath& vNodePath,
            const NodeLabel& vNodeLabel,
            const NodeType& vNodeType,
            const SlotTypes& vInputSlotTypes,             // for add a node for a slot type
            const SlotTypes& vOutputSlotTypes,            // for add a node for a slot type
            const CreateNodeFunctor& vCreateNodeFunctor,  // for the create the node
            const NodeSource vNodeSource = "INTERNAL",    // the origin of the node (the app ?, a plugin ?)
            const NodeColor& vNodeColor = {})
            : nodePath(vNodePath),
              nodeLabel(vNodeLabel),
              nodeType(vNodeType),
              inputSlotTypes(vInputSlotTypes),
              outputSlotTypes(vOutputSlotTypes),
              createNodeFunctor(vCreateNodeFunctor),
              nodeSource(vNodeSource),
              nodeColor(vNodeColor) {}
    };
    typedef std::map<CategoryName, BaseLibrary> CategoriesCnt;
    typedef std::map<NodeLabel, LibraryEntry> LibraryEntriesCnt;

private:
    CategoryName m_categoryName;
    std::map<NodeLabel, LibraryEntry> m_entries;
    CategoriesCnt m_subCategories;

    // for display on top of the popup.
    // only for categories (Inputs, Outputs) not plugins
    CategoriesCnt m_mainSubCategories;
    std::map<NodeLabel, LibraryEntry> m_mainEntries;

public:
    void clear();
    void addLibraryEntry(const LibraryEntry& vLibraryEntry);
    bool empty() const;               // no category, no sub categories, no entries
    bool emptyCategory() const;       // no category
    bool emptySubCategories() const;  // no sub categories
    bool emptyEntries() const;        // no entries

    /// <summary>
    /// return true if an entry was selected
    /// </summary>
    /// <param name="vOutEntry">the selected entry</param>
    /// <returns></returns>
    bool showMenu(LibraryEntry& vOutLibraryEntry);

    /// <summary>
    /// will remove node who not have input slot types
    /// </summary>
    /// <param name="vSlotTypes"></param>
    /// <returns></returns>
    bool filterNodesForSomeInputSlotTypes(const SlotTypes& vInputSlotTypes);

    /// <summary>
    /// will create a child node in graph
    /// </summary>
    /// <param name="vEntry"></param>
    /// <param name="vGraph"></param>
    /// <returns></returns>
    BaseNodeWeak createChildNodeInGraph(const LibraryEntry& vEntry, const BaseGraphWeak& vGraph);

private:
    BaseLibrary* m_addCategory(const CategoryName& vCategoryName, CategoriesCnt& vCategories);
    bool m_showMenu(LibraryEntry& vOutEntry, int32_t vLevel);
    bool m_showContent(LibraryEntry& vOutEntry, int32_t vLevel);
    bool m_filterNodesForSomeInputSlotTypes(const SlotTypes& vInputSlotTypes, int32_t vLevel);
    bool m_filterCategories(const SlotTypes& vInputSlotTypes, CategoriesCnt& vCategories, int32_t vLevel);
    bool m_filterEntries(const SlotTypes& vInputSlotTypes, LibraryEntriesCnt& vEntries);
};
