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
    auto* drawListPtr = ImGui::GetWindowDrawList();
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
                auto base_slot_ptr = std::dynamic_pointer_cast<BaseSlot>(ez_slot_ptr);
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
                auto base_slot_ptr = std::dynamic_pointer_cast<BaseSlot>(ez_slot_ptr);
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

void BaseNode::m_drawGraphBegin() {
    m_canvas.begin();
    m_canvas.drawGrid(m_gridConfig);
}

bool BaseNode::m_drawGraphNodes() {
    bool ret = false;
    auto* drawListPtr = ImGui::GetWindowDrawList();
    if (drawListPtr != nullptr) {
        drawListPtr->ChannelsSplit(2);
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
    m_canvas.end();
}
