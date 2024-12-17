#include <graph/baseNode.h>
#include <graph/baseSlot.h>

bool BaseNode::drawGraph() {
    bool ret = false;
    m_drawGraphBegin();
    m_drawGraphNodes();
    m_drawGraphEnd();
    return ret;
}

bool BaseNode::drawNode() {
    /*auto* drawListPtr = ImGui::GetWindowDrawList();
    if (drawListPtr != nullptr) {
        auto parentGraphPtr = m_getParentGraphPtr();
        if (parentGraphPtr != nullptr) {
            ImGui::PushID(this);
            ImVec2 localPos = parentGraphPtr->m_canvas.canvasToScreen(m_pos);
            ImVec2 paddingTL = {m_nodeConfig.padding.x, m_nodeConfig.padding.y};
            ImVec2 paddingBR = {m_nodeConfig.padding.z, m_nodeConfig.padding.w};

            drawListPtr->ChannelsSetCurrent(1);  // Foreground
            ImGui::SetCursorScreenPos(localPos);

            ImGui::BeginGroup();

            // Header
            ImGui::BeginGroup();
            m_drawNodeHeader();
            ImGui::EndGroup();
            const auto& item_size = ImGui::GetItemRectSize();

            // Inputs
            ImGui::BeginGroup();
            for (auto& ez_slot_ptr : m_getInputsRef()) {
                // the check have been done before. 
                // we are sure this is derived from baseSlot
                // so we can avoid use of slow dynamic_cast
                auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(ez_slot_ptr);
                if (base_slot_ptr != nullptr) {
                    base_slot_ptr->setPos(ImGui::GetCursorPos());
                    base_slot_ptr->draw();
                }
            }
            ImGui::EndGroup();

            ImGui::SameLine();

            // Content
            ImGui::BeginGroup();
            m_drawNodeContent();
            ImGui::Dummy(ImVec2(0.f, 0.f));
            ImGui::EndGroup();
            ImGui::SameLine();

            // Outputs
            ImGui::BeginGroup();
            for (auto& ez_slot_ptr : m_getOutputsRef()) {
                // the check have been done before.
                // we are sure this is derived from baseSlot
                // so we can avoid use of slow dynamic_cast
                auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(ez_slot_ptr);
                if (base_slot_ptr != nullptr) {
                    base_slot_ptr->setPos(ImGui::GetCursorPos());
                    base_slot_ptr->draw();
                }
            }
            ImGui::EndGroup();

            ImGui::EndGroup();
            m_size = ImGui::GetItemRectSize();
            ImVec2 headerSize = ImVec2(m_size.x + paddingBR.x, item_size.y);

            // Events
            m_isHovered = ImGui::IsMouseHoveringRect(localPos - paddingTL, localPos + headerSize);
            if (m_isHovered) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    parentGraphPtr->m_selectedNodes.clear();
                    parentGraphPtr->m_selectedNodes.emplace(getUUID());
                }
            }

            // a selection can be direct by a left click
            m_isHovered = false;
            bool m_isSelected = (parentGraphPtr->m_selectedNodes.find(getUUID()) != parentGraphPtr->m_selectedNodes.end());

            // a selection can be indirect by a zone selection
            // m_IsZoneSelected

            // Background
            drawListPtr->ChannelsSetCurrent(0);

            // node background
            drawListPtr->AddRectFilled(  //
                localPos - paddingTL,
                localPos + m_size + paddingBR,
                m_nodeConfig.bgColor,
                m_nodeConfig.cornerRadius);

            // header background
            drawListPtr->AddRectFilled(  //
                localPos - paddingTL,
                localPos + headerSize,
                m_nodeConfig.headerBgColor,
                m_nodeConfig.cornerRadius,
                ImDrawFlags_RoundCornersTop);

            ImU32 col = m_nodeConfig.borderColor;
            float thickness = m_nodeConfig.borderThickness;
            ImVec2 ptl = paddingTL;
            ImVec2 pbr = paddingBR;

            if (m_isSelected) {
                col = m_nodeConfig.borderSelectedColor;
                thickness = m_nodeConfig.borderSelectedThickness;
            } else if (m_isHovered) {
                col = m_nodeConfig.borderHoveredColor;
                thickness = m_nodeConfig.borderHoveredThickness;
            }

            // node border
            drawListPtr->AddRect(localPos - ptl, localPos + m_size + pbr, col, m_nodeConfig.cornerRadius, 0, thickness);

            ImGui::PopID();
        }
    }*/
    return false;
}

ez::RetCodes BaseNode::connectSlots(ez::SlotWeak vFrom, ez::SlotWeak vTo) {
    return m_connectSlots(vFrom, vTo);
}

bool BaseNode::m_drawNodeHeader() {
    ImGui::Text(" %s", m_nodeTitle.c_str());
    return false;
}

bool BaseNode::m_drawNodeContent() {
    return false;
}

bool BaseNode::m_drawNodeInputSlots() {
    return false;
}

bool BaseNode::m_drawNodeOutputSlots() {
    return false;
}

BaseNodeWeak BaseNode::m_getParentGraph() {
    return std::dynamic_pointer_cast<BaseNode>(getParentNode().lock());
}

BaseNodePtr BaseNode::m_getParentGraphPtr() {
    return m_getParentGraph().lock();
}

void BaseNode::m_drawGraphBegin() {
    static ImVec2 gridSize{50.0f, 50.0f};
    static ImVec2 gridSubdivs{5.0f, 5.0f};
    static ImU32 gridColor{IM_COL32(200, 200, 200, 40)};
    static ImU32 subGridColor{IM_COL32(200, 200, 200, 10)};
    
    /*if (m_canvas.Begin("##canvas", ImGui::GetContentRegionAvail())) {

        // grid
        auto* drawListPtr = ImGui::GetWindowDrawList();
        if (drawListPtr != nullptr) {
            const ImVec2 win_pos = ImGui::GetCursorScreenPos();
            const ImVec2 canvas_sz = ImGui::GetWindowSize();
            for (float x = fmodf(m_canvas.getScroll().x, vGridConfig.gridSize.x); x < canvas_sz.x; x += vGridConfig.gridSize.x) {
                drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, vGridConfig.gridColor);
            }
            for (float y = fmodf(getScroll().y, vGridConfig.gridSize.y); y < canvas_sz.y; y += vGridConfig.gridSize.y) {
                drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, vGridConfig.gridColor);
            }
            for (float x = fmodf(getScroll().x, vGridConfig.gridSize.x / vGridConfig.gridSubdivs.x); x < canvas_sz.x;
                 x += vGridConfig.gridSize.x / vGridConfig.gridSubdivs.x) {
                drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, vGridConfig.subGridColor);
            }
            for (float y = fmodf(getScroll().y, vGridConfig.gridSize.y / vGridConfig.gridSubdivs.y); y < canvas_sz.y;
                 y += vGridConfig.gridSize.y / vGridConfig.gridSubdivs.y) {
                drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, vGridConfig.subGridColor);
            }
        }

        m_canvas.End();
    }*/
    /*    auto* drawListPtr = ImGui::GetWindowDrawList();
    if (drawListPtr != nullptr) {
        const ImVec2 win_pos = ImGui::GetCursorScreenPos();
        const ImVec2 canvas_sz = ImGui::GetWindowSize();
        for (float x = fmodf(getScroll().x, vGridConfig.gridSize.x); x < canvas_sz.x; x += vGridConfig.gridSize.x) {
            drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, vGridConfig.gridColor);
        }
        for (float y = fmodf(getScroll().y, vGridConfig.gridSize.y); y < canvas_sz.y; y += vGridConfig.gridSize.y) {
            drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, vGridConfig.gridColor);
        }
        for (float x = fmodf(getScroll().x, vGridConfig.gridSize.x / vGridConfig.gridSubdivs.x); x < canvas_sz.x;
             x += vGridConfig.gridSize.x / vGridConfig.gridSubdivs.x) {
            drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, vGridConfig.subGridColor);
        }
        for (float y = fmodf(getScroll().y, vGridConfig.gridSize.y / vGridConfig.gridSubdivs.y); y < canvas_sz.y;
             y += vGridConfig.gridSize.y / vGridConfig.gridSubdivs.y) {
            drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, vGridConfig.subGridColor);
        }
    }*/
}

bool BaseNode::m_drawGraphNodes() {
    bool ret = false;
    auto* drawListPtr = ImGui::GetWindowDrawList();
    if (drawListPtr != nullptr) {
        drawListPtr->ChannelsSplit(3);
        for (auto& node_ptr : getNodes()) {
            auto base_node_ptr = std::dynamic_pointer_cast<BaseNode>(node_ptr);
            if (base_node_ptr != nullptr) {
                ret |= base_node_ptr->drawNode();
            }
        }
        drawListPtr->ChannelsMerge();
    }
    return ret;
}

void BaseNode::m_drawGraphEnd() {
    //m_canvas.end();
}
