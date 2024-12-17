#pragma once

#include <imguipack/ImGuiPack.h>
#include <graph/baseNode.h>
#include <graph/baseCanvas.h>

#include <memory>
#include <functional>

class NodeManager {
private: // Static
    static std::unique_ptr<NodeManager> m_singleton;

private: // Normal
    BaseCanvas m_canvas;
    int m_externalChannel{0};
    bool m_backgroundClicked{false};
    bool m_backgroundDoubleClicked{false};
    bool m_isDragging{false};
    ImVec2 m_drawStartPoint;
    float m_zoomSpeed{0.1f};
    ImU32 m_backgroundColor{IM_COL32(55, 64, 75, 255)};
    ImVec4 m_gridColor{200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 40.0f / 255.0f};

    struct GridConfig {
        ImVec2 gridSize{50.0f, 50.0f};
        ImVec2 gridSubdivs{5.0f, 5.0f};
        ImU32 gridColor{IM_COL32(200, 200, 200, 40)};
        ImU32 subGridColor{IM_COL32(200, 200, 200, 10)};
    } m_gridConfig;

public:
    static NodeManager* instance();
    static bool initInstance();
    static void unitInstance();

public:
    bool init();
    void unit();
    bool drawGraph();
    void suspend();
    void resume();

private:
    void m_manageActions();
    void m_manageZoom();
    void m_manageDragging();
    void m_drawGridAndBackground();
};
