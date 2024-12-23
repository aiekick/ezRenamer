#pragma once

#include <imguipack/ImGuiPack.h>
#include <graph/baseStyle.h>
#include <graph/baseGraph.h>

#include <memory>
#include <functional>

class NodeManager {
private: // Static
    static std::unique_ptr<NodeManager> m_singleton;

private: // Normal
    BaseStyle m_baseStyle;
    BaseGraph::BaseGraphDatas m_graphConfig;
    BaseGraphPtr m_graphPtr = nullptr;
    std::map<std::string, ImVec4> m_ColorSlots;

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
};
