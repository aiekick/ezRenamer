#pragma once

#include <ImGuiPack.h>

struct BaseCanvasView {
    ImVec2 Origin;
    float Scale = 1.0f;
    float InvScale = 1.0f;
    BaseCanvasView() = default;
    BaseCanvasView(const ImVec2& origin, float scale) : Origin(origin), Scale(scale), InvScale(scale ? 1.0f / scale : 0.0f) {}
    void Set(const ImVec2& origin, float scale) { *this = BaseCanvasView(origin, scale); }
};

class BaseCanvas {
public:
    bool Begin(const char* id, const ImVec2& size);
    bool Begin(ImGuiID id, const ImVec2& size);
    void End();

    void SetView(const ImVec2& origin, float scale);
    void SetView(const BaseCanvasView& view);
    void CenterView(const ImVec2& canvasPoint);
    BaseCanvasView CalcCenterView(const ImVec2& canvasPoint) const;
    void CenterView(const ImRect& canvasRect);
    BaseCanvasView CalcCenterView(const ImRect& canvasRect) const;

    void Suspend();
    bool IsSuspended() const { return m_SuspendCounter > 0; }
    void Resume();

    ImVec2 canvasToScreen(const ImVec2& point) const;
    ImVec2 canvasToScreen(const ImVec2& point, const BaseCanvasView& view) const;

    ImVec2 canvasToScreenV(const ImVec2& vector) const;
    ImVec2 canvasToScreenV(const ImVec2& vector, const BaseCanvasView& view) const;

    ImVec2 screenToCanvas(const ImVec2& point) const;
    ImVec2 screenToCanvas(const ImVec2& point, const BaseCanvasView& view) const;

    ImVec2 screenToCanvasV(const ImVec2& vector) const;
    ImVec2 screenToCanvasV(const ImVec2& vector, const BaseCanvasView& view) const;

    const ImRect& Rect() const { return m_WidgetRect; }
    const ImRect& ViewRect() const { return m_ViewRect; }
    ImRect CalcViewRect(const BaseCanvasView& view) const;
    const BaseCanvasView& View() const { return m_View; }
    BaseCanvasView& ViewRef() { return m_View; }
    const ImVec2& ViewOrigin() const { return m_View.Origin; }
    float ViewScale() const { return m_View.Scale; }

private:
    void m_updateViewTransformPosition();

    void m_saveInputState();
    void m_restoreInputState();

    void m_saveViewportState();
    void m_restoreViewportState();

    void m_enterLocalSpace();
    void m_leaveLocalSpace();

    bool m_InBeginEnd = false;

    ImVec2 m_WidgetPosition;
    ImVec2 m_WidgetSize;
    ImRect m_WidgetRect;

    ImDrawList* m_DrawList = nullptr;
    int m_ExpectedChannel = 0;

    int m_DrawListCommadBufferSize = 0;
    int m_DrawListStartVertexIndex = 0;

    BaseCanvasView m_View;
    ImRect m_ViewRect;

    ImVec2 m_ViewTransformPosition;

    int m_SuspendCounter = 0;

    float m_LastFringeScale = 1.0f;

    ImVec2 m_MousePosBackup;
    ImVec2 m_MousePosPrevBackup;
    ImVec2 m_MouseClickedPosBackup[IM_ARRAYSIZE(ImGuiIO::MouseClickedPos)];
    ImVec2 m_WindowCursorMaxBackup;

    ImVec2 m_WindowPosBackup;
    ImVec2 m_ViewportPosBackup;
    ImVec2 m_ViewportSizeBackup;
    ImVec2 m_ViewportWorkPosBackup;
    ImVec2 m_ViewportWorkSizeBackup;
};
