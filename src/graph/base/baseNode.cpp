#include "baseNode.h"
#include <ezlibs/ezLog.hpp>

bool BaseNode::drawWidgets(const uint32_t& vFrame) {
    return false;
}

bool BaseNode::drawNodeWidget(const uint32_t& vFrame) {
    return false;
}

bool BaseNode::drawNode() {
    bool change = false;
    if (m_drawBegin()) {
        change |= m_drawHeader();
        m_headerRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        change |= m_drawContent();
        change |= m_drawFooter();
        change |= m_drawEnd();
    }

    m_size = ImGui::GetItemRectSize();
    m_pos = ImGui::GetItemRectMin();
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        }
    }
    return change;
}

ez::xml::Nodes BaseNode::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node node;
    return node.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool BaseNode::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    return true;
}

const BaseStyle& BaseNode::getParentStyle() {
    return m_parentStyle;
}

BaseStyle BaseNode::getNodeStyle() {
    return m_nodeStyle;
}

BaseSlotWeak BaseNode::findSlotByType(ez::SlotDir vDir, const std::string& vType) {
    BaseSlotWeak ret;
    if (!vType.empty()) {
        if (vDir == ez::SlotDir::INPUT) {
            for (const auto& slot : m_getInputSlots()) {
                auto base_pin_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
                if (base_pin_ptr->getDatas<BaseSlot::BaseSlotDatas>().type == vType) {
                    ret = base_pin_ptr;
                    break;
                }
            }
        } else if (vDir == ez::SlotDir::OUTPUT) {
            for (const auto& slot : m_getOutputSlots()) {
                auto base_pin_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
                if (base_pin_ptr->getDatas<BaseSlot::BaseSlotDatas>().type == vType) {
                    ret = base_pin_ptr;
                    break;
                }
            }
        }
    }
    return ret;
}

bool BaseNode::m_drawBegin() {
    nd::PushStyleVar(nd::StyleVar_NodePadding, ImVec4(4, 2, 4, 4));
    nd::BeginNode(m_nodeID);
    ImGui::PushID(m_nodeID.AsPointer());
    ImGui::BeginVertical("node");
    return true;
}

bool BaseNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    ImGui::Spring(1, 5.0f);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    ImGui::Spring(1, 5.0f);
    ImGui::Dummy(ImVec2(0, 20));
    ImGui::EndHorizontal();
    return false;
}

bool BaseNode::m_drawContent() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));
    ImGui::BeginHorizontal("content");
    ImGui::Spring(0, 0);
    ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);
    for (auto& slot : m_getInputSlotsRef()) {  // slots
        std::static_pointer_cast<BaseSlot>(slot.lock())->draw();
    }
    ImGui::EndVertical();
    ImGui::Spring(1, 5.0f);                               // pour que BeginVertical soi poussé au bout
    ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);  // 1.0f pour que l'interieur soit aligné sur la fin
    for (auto& slot : m_getOutputSlotsRef()) {            // slots
        std::static_pointer_cast<BaseSlot>(slot.lock())->draw();
    }
    ImGui::EndVertical();
    ImGui::EndHorizontal();
    ImGui::PopStyleVar();
    return false;
}

bool BaseNode::m_drawInputSlots() {
    return false;
}

bool BaseNode::m_drawOutputSlots() {
    return false;
}

bool BaseNode::m_drawFooter() {
    return false;
}

bool BaseNode::m_drawEnd() {
    bool ret = false;
    ImGui::EndVertical();
    nd::EndNode();
    if (ImGui::IsItemVisible()) {
        auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);
        auto drawList = nd::GetNodeBackgroundDrawList(m_nodeID);
        if (drawList) {
            ImGuiContext& g = *GImGui;
            const auto itemRect = g.LastItemData.Rect;
            if (m_headerRect.GetSize().y > 0.0f) {
                const ImVec4 NodePadding = nd::GetStyle().NodePadding;
                const auto halfBorderWidth = nd::GetStyle().NodeBorderWidth;
                drawList->AddRectFilled(
                    m_headerRect.Min - ImVec2(NodePadding.x - halfBorderWidth, NodePadding.y - halfBorderWidth),
                    m_headerRect.Max + ImVec2(NodePadding.z - halfBorderWidth, 0),
                    getDatas<BaseNodeDatas>().color,
                    nd::GetStyle().NodeRounding,
                    ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight);
                auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);
                drawList->AddLine(
                    ImVec2(m_headerRect.Min.x - (NodePadding.x - halfBorderWidth), m_headerRect.Max.y - 0.5f),
                    ImVec2(m_headerRect.Max.x + (NodePadding.z - halfBorderWidth), m_headerRect.Max.y - 0.5f),
                    ImColor(255, 255, 255, 96 * alpha / (3 * 255)),
                    1.0f);
            }

            ret |= m_drawHints();
        } else {
            EZ_TOOLS_DEBUG_BREAK;
        }
    }
    ImGui::PopID();
    nd::PopStyleVar();
    return ret;
}

bool BaseNode::m_drawHints() {
    for (auto& slot : m_getInputSlotsRef()) {
        auto ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
        if (nd::GetHoveredPin().Get() == ptr->getUuid()) {
            ptr->m_drawHoveredSlotText(ptr->m_pos, false, 0, 0);
        }
    }

    for (auto& slot : m_getOutputSlotsRef()) {
        auto ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
        if (nd::GetHoveredPin().Get() == ptr->getUuid()) {
            ptr->m_drawHoveredSlotText(ptr->m_pos, false, 0, 0);
        }
    }
    return false;
}

void BaseNode::m_displayInfosOnTopOfTheNode() {
    auto drawList = nd::GetNodeBackgroundDrawList(m_nodeID);
    if (drawList) {
        auto datas = getDatas<BaseNodeDatas>();
        const std::string& debugInfos = ez::str::toStr("Used(%s)\nCell(%i, %i)", datas.layout.used, datas.layout.cell.x, datas.layout.cell.y);
        ImVec2 txtSize = ImGui::CalcTextSize(debugInfos.c_str());
        drawList->AddText(m_pos - ImVec2(0, txtSize.y), ImGui::GetColorU32(ImGuiCol_Text), debugInfos.c_str());
    }
}

BaseSlotWeak BaseNode::m_findSlot(nd::PinId vId) {
    BaseSlotWeak ret;
    if (vId) {
        for (const auto& slot : m_getInputSlots()) {
            auto base_pin_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
            if (base_pin_ptr->m_pinID == vId) {
                ret = base_pin_ptr;
                break;
            }
        }
        for (const auto& slot : m_getOutputSlots()) {
            auto base_pin_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
            if (base_pin_ptr->m_pinID == vId) {
                ret = base_pin_ptr;
                break;
            }
        }
    }
    return ret;
}