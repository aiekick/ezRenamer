#include <graph/nodeManager.h>
#include <graph/baseSlot.h>

std::unique_ptr<NodeManager> NodeManager::m_singleton = nullptr;

NodeManager* NodeManager::instance() {
    assert(m_singleton != nullptr);
    return m_singleton.get();
}

bool NodeManager::initInstance() {
    m_singleton = std::make_unique<NodeManager>();
    return instance()->init();
}

void NodeManager::unitInstance() {
    instance()->unit();
    m_singleton.reset();
}

bool NodeManager::init() {
    /*m_graphPtr = BaseNode::create({});
    auto node = m_graphPtr->createChildNode<BaseNode>({});
    auto ptr = node.lock();
    if (ptr != nullptr) {
        ptr->addSlot<BaseSlot>(BaseSlot::SlotConfig("in0", "base", ez::SlotDir::INPUT));
        ptr->addSlot<BaseSlot>(BaseSlot::SlotConfig("out0", "base", ez::SlotDir::OUTPUT));
    }*/
    return true;
}

void NodeManager::unit() {
    //m_graphPtr.reset();
}

bool NodeManager::drawGraph() {
    bool change = false;
    auto availableContentSize = ImGui::GetContentRegionAvail();
    if (m_canvas.Begin("canvas", availableContentSize)) {
        m_manageActions();
        m_drawGridAndBackground();
        m_canvas.End();
    }
    return change;
}

void NodeManager::suspend() {
    auto drawList = ImGui::GetWindowDrawList();
    auto lastChannel = drawList->_Splitter._Current;
    drawList->ChannelsSetCurrent(m_externalChannel);
    m_canvas.Suspend();
    drawList->ChannelsSetCurrent(lastChannel);
}

void NodeManager::resume() {
    auto drawList = ImGui::GetWindowDrawList();
    auto lastChannel = drawList->_Splitter._Current;
    drawList->ChannelsSetCurrent(m_externalChannel);
    m_canvas.Resume();
    drawList->ChannelsSetCurrent(lastChannel);
}

void NodeManager::m_manageActions() {
    if (ImGui::IsWindowHovered() && ImGui::IsWindowFocused()) {
        m_manageZoom();
        m_manageDragging();
    }
}

void NodeManager::m_manageZoom() {
    const auto& viewOrigin = m_canvas.ViewOrigin();
    const auto& viewRect = m_canvas.ViewRect();
    auto viewScale = m_canvas.ViewScale();
    const auto& io = ImGui::GetIO();
    auto mousePos = io.MousePos;
    auto steps = (int)io.MouseWheel;
    if (IS_FLOAT_DIFFERENT(steps, 0.0f)) {
        auto oldView = BaseCanvasView(viewOrigin, viewScale);
        viewScale = ImMax(viewScale + steps * m_zoomSpeed, 0.1f);
        auto newView = BaseCanvasView(viewOrigin, viewScale);
        auto screenPos = m_canvas.canvasToScreen(mousePos, oldView);
        auto canvasPos = m_canvas.screenToCanvas(screenPos, newView);
        auto offset = (canvasPos - mousePos) * viewScale;
        auto targetScroll = viewOrigin + offset;
        m_canvas.SetView(targetScroll, viewScale);
    }
}

void NodeManager::m_manageDragging() {
    const auto& viewOrigin = m_canvas.ViewOrigin();
    const auto viewScale = m_canvas.ViewScale();
    if ((m_isDragging || ImGui::IsItemHovered()) && ImGui::IsMouseDragging(0, 0.0f)) {
        if (!m_isDragging) {
            m_isDragging = true;
            m_drawStartPoint = viewOrigin;
        }
        m_canvas.SetView(m_drawStartPoint + ImGui::GetMouseDragDelta(0, 0.0f) * viewScale, viewScale);
    } else if (m_isDragging) {
        m_isDragging = false;
    }
}

void NodeManager::m_drawGridAndBackground() {
    auto* drawListPtr = ImGui::GetWindowDrawList();
    const ImVec2 offset = m_canvas.ViewOrigin() / m_canvas.ViewScale();
    if (drawListPtr != nullptr) {
        const ImVec2 win_pos = m_canvas.ViewRect().Min;
        const ImVec2 canvas_sz = m_canvas.ViewRect().GetSize();
        for (float x = fmodf(offset.x, m_gridConfig.gridSize.x); x < canvas_sz.x; x += m_gridConfig.gridSize.x) {
            drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, m_gridConfig.gridColor);
        }
        for (float y = fmodf(offset.y, m_gridConfig.gridSize.y); y < canvas_sz.y; y += m_gridConfig.gridSize.y) {
            drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, m_gridConfig.gridColor);
        }
        for (float x = fmodf(offset.x, m_gridConfig.gridSize.x / m_gridConfig.gridSubdivs.x); x < canvas_sz.x;
             x += m_gridConfig.gridSize.x / m_gridConfig.gridSubdivs.x) {
            drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, m_gridConfig.subGridColor);
        }
        for (float y = fmodf(offset.y, m_gridConfig.gridSize.y / m_gridConfig.gridSubdivs.y); y < canvas_sz.y;
             y += m_gridConfig.gridSize.y / m_gridConfig.gridSubdivs.y) {
            drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, m_gridConfig.subGridColor);
        }
    }
}
