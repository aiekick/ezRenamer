#pragma once

#include <imguipack/ImGuiPack.h>
#include <graph/base/baseDefs.h>
#include <graph/base/baseGraph.h>
#include <graph/base/baseLibrary.h>
#include <graph/base/interfaces/SlotColorBankInterface.h>

#include <memory>
#include <functional>

class NodeManager : public SlotColorBankInterface, public IDrawDebugInfos {
private: // Static
    static std::unique_ptr<NodeManager> mp_singleton;

public:  // Static
    static NodeManager* instance();
    static bool initInstance();
    static void unitInstance();

private: 
    BaseStyle m_graphStyle;
    BaseGraph::BaseGraphDatas m_graphConfig;
    BaseGraphPtr m_graphPtr = nullptr;
    std::map<std::string, ImVec4> m_ColorSlots;
    BaseLibrary m_nodesLibrary;

    // Library to show, can be filtered from m_nodesLibrary or not
    BaseLibrary m_libraryToShow;

    // used to create a node from this slot and connect
    // the input slot af the newx node to this slot
    // empty if not createFromSlot mode
    BaseSlotWeak m_createNodeFromSlot;

public:
    bool init();
    void unit();
    bool drawGraph();
    BaseGraphWeak getGraph() const;
    bool getSlotColor(const std::string& vBaseSlotType, ImVec4& vOutColor) const override;
    bool getSlotColor(const std::string& vBaseSlotType, ImU32& vOutColor) const override;
    void addSlotColor(const std::string& vBaseSlotType, const ImVec4& vSlotColor) override;
    void drawDebugInfos() override;

private:
    bool m_filterLibraryForInputSlotType(const BaseLibrary::SlotType& vSlotType);
    void m_showLibrary();
};
