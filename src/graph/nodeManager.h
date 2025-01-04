#pragma once

#include <imguipack/ImGuiPack.h>
#include <graph/base/baseStyle.h>
#include <graph/base/baseGraph.h>
#include <graph/base/baseLibrary.h>

#include <memory>
#include <functional>

class NodeManager {
private: // Static
    static std::unique_ptr<NodeManager> m_singleton;

private: 
    BaseStyle m_graphStyle;
    BaseGraph::BaseGraphDatas m_graphConfig;
    BaseGraphPtr m_graphPtr = nullptr;
    std::map<std::string, ImVec4> m_ColorSlots;
    BaseLibrary m_baseLibrary;

    // Library to show, can be filtered from m_baseLibrary or not
    BaseLibrary m_libraryToShow;

    // used to create a node from this slot and connect
    // the input slot af the newx node to this slot
    // empty if not createFromSlot mode
    BaseSlotWeak m_createNodeFromSlot;

public:
    static NodeManager* instance();
    static bool initInstance();
    static void unitInstance();

public:
    bool init();
    void unit();
    bool drawGraph();
    ImVec4 getSlotColor(const std::string& vBaseSlotType);
    void addSlotColor(const std::string& vBaseSlotType, const ImVec4& vSlotColor);

private:
    bool m_filterLibraryForInputSlotType(const BaseLibrary::SlotType& vSlotType);
    void m_showLibrary();
    BaseNodeWeak m_createInternalNode(const BaseLibrary::LibraryEntry& vLibraryEntry);
    BaseNodeWeak m_createPluginNode(const BaseLibrary::LibraryEntry& vLibraryEntry);
};
