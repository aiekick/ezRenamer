#pragma once

#include <imguipack/imguipack.h>

#include <grapher/grapher.h>

#include <grapher/interfaces/SlotColorBankInterface.h>

#include <ezlibs/ezXmlConfig.hpp>

#include <memory>
#include <functional>

class NodeManager : public SlotColorBankInterface, public IDrawDebugInfos, public ez::xml::Config {
private: // Static
    static std::unique_ptr<NodeManager> mp_singleton;

public:  // Static
    static NodeManager* Instance();
    static bool initInstance();
    static void unitInstance();

private: 
    BaseStyle m_graphStyle;
    BaseGraph::BaseGraphDatas m_graphConfig;
    BaseGraphPtr m_graphPtr = nullptr;
    std::map<std::string, ImVec4> m_ColorSlots;
    BaseLayout m_baseLayout;

    BaseLibrary m_nodesLibrary;
    // Library to show, can be filtered from m_nodesLibrary or not
    BaseLibrary m_libraryToShow;

    // used to create a node from this slot and connect
    // the input slot af the newx node to this slot
    // empty if not createFromSlot mode
    BaseSlotWeak m_createNodeFromSlot;

    BaseNodeWeak m_selectedNode;

public:
    bool init();
    void unit();
    void clear();
    bool drawGraph();
    bool drawControl();
    BaseGraphWeak getGraph() const;
    bool getSlotColor(const std::string& vBaseSlotType, ImVec4& vOutColor) const override;
    bool getSlotColor(const std::string& vBaseSlotType, ImU32& vOutColor) const override;
    void addSlotColor(const std::string& vBaseSlotType, const ImVec4& vSlotColor) override;
    void drawDebugInfos() override;
    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas) override ;
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;
    BaseNodeWeak createChildNodeInGraph(const BaseLibrary::NodeType& vNodeType, const BaseGraphWeak& vGraph);
    void beforeXmlLoading();
    void afterXmlLoading();

private:
    void m_selectNode(const BaseGraphWeak& vGraph, const BaseNodeWeak& vNode);
    bool m_loadNodeFromXml(const BaseGraphWeak& vGraph, const ez::xml::Node& vNode, const ez::xml::Node& vParent);
    bool m_filterLibraryForInputSlotType(const BaseLibrary::SlotType& vSlotType);
    void m_showLibrary();
};
