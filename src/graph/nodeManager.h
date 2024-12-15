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

public:
    static NodeManager* instance();
    static bool initInstance();
    static void unitInstance();

public:
    bool init();
    void unit();
    bool drawGraph();
};
