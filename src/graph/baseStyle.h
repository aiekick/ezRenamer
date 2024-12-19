#pragma once

#include <ImGuiPack.h>

struct CommonStyle {
    // node
    float DEFAULT_WIDTH = 100.0f;
    float WINDOW_PADDING = 10.0f;
    float BACKGROUND_RADIUS = 0.5f;
    ImVec4 HEADER_COLOR = ImVec4(0.05f, 0.05f, 0.95f, 1.0f);
    ImVec4 HOVERED_HEADER_COLOR = ImVec4(0.1f, 0.1f, 0.95f, 1.0f);
    ImVec4 BACKGROUND_COLOR = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImVec4 HOVERED_BACKGROUND_COLOR = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

    // slot
    float SLOT_RADIUS = 5.0f;
    ImVec4 SLOT_COLOR = ImVec4(0.8f, 0.8f, 0.2f, 1.0f);
    ImVec4 FLOW_SLOT_COLOR = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 FUNCTION_SLOT_COLOR = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

    // grid
    ImVec4 GRID_COLOR = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
    ImVec4 GRID_COLOR_ZERO_X = ImVec4(0.8f, 0.2f, 0.2f, 0.8f);
    ImVec4 GRID_COLOR_ZERO_Y = ImVec4(0.2f, 0.8f, 0.2f, 0.8f);
    float GRID_SPACING = 64.0f;

    // link
    ImVec4 linkDefaultColor = ImVec4(0.4f, 0.6f, 0.3f, 1.0f);
    ImVec4 selectedLinkColor = ImVec4(0.5f, 0.8f, 0.5f, 1.0f);
    ImVec4 extractionLinkColor = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
    ImVec4 selectedExtractionLinkColor = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);

    // graph
    nd::Style graphStyle;
};
