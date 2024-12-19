#pragma once

#include <imguipack/ImGuiPack.h>
#include <graph/baseNode.h>

#include <memory>
#include <functional>

class NodeManager {
private: // Static
    static std::unique_ptr<NodeManager> m_singleton;

private: // Normal
    BaseNodePtr m_graphPtr = nullptr;
    std::map<std::string, ImVec4> m_ColorSlots;

public:
    static NodeManager* instance();
    static bool initInstance();
    static void unitInstance();

public:
    bool init();
    void unit();
    bool drawGraph();
    ImVec4 getSlotColor(const std::string& vNodeSlotType);
    void addSlotColor(const std::string& vNodeSlotType, const ImVec4& vSlotColor);
};
