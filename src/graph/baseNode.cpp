#include <graph/baseNode.h>

bool BaseNode::drawGraph() {
    m_canvas.begin();
    m_canvas.drawGrid(m_gridConfig);
    m_canvas.end();
    return false;
}

bool BaseNode::drawNode() {
    auto* drawListPtr = ImGui::GetWindowDrawList();
    if (drawListPtr != nullptr) {
        auto parentGraphPtr = m_getParentGraphPtr();
        if (parentGraphPtr != nullptr) {
            ImGui::PushID(this);
            ImVec2 offset = parentGraphPtr->m_canvas.canvasToScreen(ImVec2(0.0f, 0.0f));
            ImVec2 localPos = offset + m_pos;
            ImVec2 paddingTL = {m_nodeConfig.padding.x, m_nodeConfig.padding.y};
            ImVec2 paddingBR = {m_nodeConfig.padding.z, m_nodeConfig.padding.w};

            drawListPtr->ChannelsSetCurrent(1);  // Foreground
            ImGui::SetCursorScreenPos(localPos);

            ImGui::BeginGroup();

            // Header
            ImGui::BeginGroup();
            m_drawNodeHeader();
            ImGui::EndGroup();
            float headerH = ImGui::GetItemRectSize().y;
            // float titleW = ImGui::GetItemRectSize().x;

            // Inputs
            /*ImGui::BeginGroup();
            for (auto& p : m_ins) {
                p->setPos(ImGui::GetCursorPos());
                p->update();
            }
            ImGui::EndGroup();
            ImGui::SameLine();*/

            // Content
            /*ImGui::BeginGroup();
            draw();
            ImGui::Dummy(ImVec2(0.f, 0.f));
            ImGui::EndGroup();
            ImGui::SameLine();

            // Outputs
            float maxW = 0.0f;
            for (auto& p : m_outs) {
                float w = p->calcWidth();
                if (w > maxW)
                    maxW = w;
            }
            ImGui::BeginGroup();
            for (auto& p : m_outs) {
                // FIXME: This looks horrible
                if ((m_pos + ImVec2(titleW, 0) + m_inf->getGrid().scroll()).x < ImGui::GetCursorPos().x + ImGui::GetWindowPos().x + maxW)
                    p->setPos(ImGui::GetCursorPos() + ImGui::GetWindowPos() + ImVec2(maxW - p->calcWidth(), 0.f));
                else
                    p->setPos(ImVec2((m_pos + ImVec2(titleW - p->calcWidth(), 0) + m_inf->getGrid().scroll()).x, ImGui::GetCursorPos().y + ImGui::GetWindowPos().y));
                p->update();
            }

            ImGui::EndGroup();*/

            ImGui::EndGroup();
            m_size = ImGui::GetItemRectSize();
            ImVec2 headerSize = ImVec2(m_size.x + paddingBR.x, headerH);

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

            // mark node as "to destroy" if delete key was pressed
            /*if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !ImGui::IsAnyItemActive() && m_IsSelected) {
                parentGraphPtr->destroyNode(m_Uuid);
            }*/

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

            if (m_isSelected /*|| m_isZoneSelected*/) {
                col = m_nodeConfig.borderSelectedColor;
                thickness = m_nodeConfig.borderSelectedThickness;
            } else if (m_isHovered) {
                col = m_nodeConfig.borderHoveredColor;
                thickness = m_nodeConfig.borderHoveredThickness;
            }

            // node border
            drawListPtr->AddRect(localPos - ptl, localPos + m_size + pbr, col, m_nodeConfig.cornerRadius, 0, thickness);

            /*if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !m_inf->on_selected_node()) {
                selected(false);
            }*/

            /*if (isHovered()) {
                m_inf->hoveredNode(this);
                if (mouseClickState) {
                    selected(true);
                    m_inf->consumeSingleUseClick();
                }
            }*/

            /*if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !ImGui::IsAnyItemActive() && isSelected())
                destroy();*/

            /*bool onHeader = ImGui::IsMouseHoveringRect(localPos - paddingTL, localPos + headerSize);
            if (onHeader && mouseClickState) {
                m_inf->consumeSingleUseClick();
                m_dragged = true;
                m_inf->draggingNode(true);
            }
            if (m_dragged || (m_selected && m_inf->isNodeDragged())) {
                float step = m_inf->getStyle().grid_size / m_inf->getStyle().grid_subdivisions;
                m_posTarget += ImGui::GetIO().MouseDelta;
                // "Slam" The position
                m_pos.x = round(m_posTarget.x / step) * step;
                m_pos.y = round(m_posTarget.y / step) * step;

                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    m_dragged = false;
                    m_inf->draggingNode(false);
                    m_posTarget = m_pos;
                }
            }*/
            ImGui::PopID();

            // Resolve output pins values
            /*for (auto& p : m_outs)
                p->resolve();
            for (auto& p : m_dynamicOuts)
                p.second->resolve();*/

            // Deleting dead pins
            /*m_dynamicIns.erase(std::remove_if(m_dynamicIns.begin(), m_dynamicIns.end(), [](const std::pair<int, std::shared_ptr<Pin>>& p) { return p.first == 0; }),
                               m_dynamicIns.end());
            m_dynamicOuts.erase(std::remove_if(m_dynamicOuts.begin(), m_dynamicOuts.end(), [](const std::pair<int, std::shared_ptr<Pin>>& p) { return p.first == 0; }),
                                m_dynamicOuts.end());*/
        }
    }
    return false;
}

ez::RetCodes BaseNode::connectSlots(ez::SlotWeak vFrom, ez::SlotWeak vTo) {
    return m_connectSlots(vFrom, vTo);
}

bool BaseNode::m_drawNodeHeader() {
    ImGui::Text("%s", m_nodeTitle.c_str());
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
