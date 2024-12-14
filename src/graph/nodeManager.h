#pragma once

#include <imguipack/ImGuiPack.h>

#include <memory>
#include <functional>

class NodeManager {
private: // Static
    static std::unique_ptr<NodeManager> m_singleton;

private: // Normal
    ImCanvas::GridConfig m_gridConfig;
    ImCanvas m_canvas;

public:
    static NodeManager* instance();
    static bool initInstance();
    static void unitInstance();

public:
    bool init();
    void unit();
    bool drawGraph();
};
