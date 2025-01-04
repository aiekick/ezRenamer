#pragma once

#include <set>
#include <map>
#include <string>
#include <cstdint>
#include <ezlibs/ezMath.hpp>

class BaseLibrary {
public:
    enum class NodeSource { INTERNAL = 0, PLUGIN, Count };
    typedef std::string NodeLabel;
    typedef std::string NodeType;
    typedef std::string NodePath;
    typedef ez::fvec4 NodeColor;
    typedef std::string CategoryName;
    typedef std::string SlotType;
    typedef std::set<SlotType> SlotTypes;
    struct LibraryEntry {
        NodeSource nodeSource;
        NodePath nodePath;  // can be a path like cat0/cat1/node
        NodeLabel nodeLabel;
        NodeType nodeType;
        NodeColor nodeColor;
        SlotTypes inputSlotTypes;
        SlotTypes outputSlotTypes;
        LibraryEntry() = default;
        LibraryEntry(
            const NodePath& vNodePath,
            const NodeLabel& vNodeLabel,
            const NodeType& vNodeType,
            const SlotTypes& vInputSlotTypes = {},         // for add a node for a slot type
            const SlotTypes& vOutputSlotTypes = {},  // for add a node for a slot type
            const NodeSource vNodeSource = NodeSource::PLUGIN,
            const NodeColor& vNodeColor = {})
            : nodePath(vNodePath),
              nodeLabel(vNodeLabel),
              nodeType(vNodeType),
              inputSlotTypes(vInputSlotTypes),
              outputSlotTypes(vOutputSlotTypes),
              nodeSource(vNodeSource),
              nodeColor(vNodeColor) {}
    };

private:
    std::map<NodeLabel, LibraryEntry> m_libraryItems;
    std::map<CategoryName, BaseLibrary> m_subCategories;
    CategoryName m_categoryName;

public:
    void clear();
    void addLibraryEntry(const LibraryEntry& vLibraryEntry);

    bool empty() const;
    bool emptyLeafs() const;

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

private:
    BaseLibrary* m_addCategory(const CategoryName& vCategoryName);
    bool m_showMenu(LibraryEntry& vOutEntry, int32_t vLevel);
    bool m_showContent(LibraryEntry& vOutEntry, int32_t vLevel);
    bool m_filterNodesForSomeInputSlotTypes(const SlotTypes& vInputSlotTypes, int32_t vLevel);
};
