#include "baseNode.h"
#include <ezlibs/ezLog.hpp>

#include <graph/base/baseGraph.h>
#include <graph/base/baseSlot.h>
#include <graph/base/baseLink.h>

bool BaseNode::init() {
    if (ez::Node::init()) {
        return true;
    }
    return false;
}

bool BaseNode::drawWidgets() {
    return false;
}

bool BaseNode::drawNodeWidget(const uint32_t &vFrame) {
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

ez::xml::Nodes BaseNode::getXmlNodes(const std::string &vUserDatas) {
    ez::xml::Node xml;
    const auto &node_datas = getDatas<BaseNodeDatas>();
    auto &node = xml.addChild("node")                        //
            .addAttribute("name", node_datas.name)  //
            .addAttribute("type", node_datas.type)  //
            .addAttribute("pos", m_pos);
    // module is added here, before slots, will be filled in derived nodes
    node.addChild("module").addAttribute("type", getDatas<BaseNodeDatas>().type);
    if (!m_getInputSlots().empty()) {
        auto &slots_in = node.addChild("inputs");
        uint32_t idx = 0U;
        for (const auto &slot: m_getInputSlots()) {
            auto slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
            const auto &slot_datas = slot_ptr->getDatas<BaseSlot::BaseSlotDatas>();
            slots_in.addChild("slot")
                    .addAttribute("name", slot_datas.name)     //
                    .addAttribute("type", slot_datas.type)     //
                    .addAttribute("gid", slot_ptr->getUuid())  //
                    .addAttribute("lid", idx++);
        }
    }
    if (!m_getOutputSlots().empty()) {
        uint32_t idx = 0U;
        auto &slots_out = node.addChild("outputs");
        for (const auto &slot: m_getOutputSlots()) {
            auto slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
            const auto &slot_datas = slot_ptr->getDatas<BaseSlot::BaseSlotDatas>();
            slots_out.addChild("slot")
                    .addAttribute("name", slot_datas.name)     //
                    .addAttribute("type", slot_datas.type)     //
                    .addAttribute("gid", slot_ptr->getUuid())  //
                    .addAttribute("lid", idx++);
        }
    }
    return xml.getChildren();
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool
BaseNode::setFromXmlNodes(const ez::xml::Node &vNode, const ez::xml::Node &vParent, const std::string &vUserDatas) {
    const auto &strName = vNode.getName();
    const auto &strParentName = vParent.getName();
    if (strName == "node") {
        const auto &node_datas = getDatas<BaseNodeDatas>();
        m_pos = vNode.getAttribute<ImVec2>("pos");
        nd::SetNodePosition(m_nodeID, m_pos);
    } else if (strName == "slot") {
        if (strParentName == "inputs") {
            uint32_t idx = 0U;
            for (const auto &slot: m_getInputSlots()) {
                auto slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
                if (vNode.getAttribute("lid") == ez::str::toStr("%u", idx++)) {
                    slot_ptr->setUuid(vNode.getAttribute<ez::Uuid>("gid"));
                }
            }
        }
        if (strParentName == "outputs") {
            uint32_t idx = 0U;
            for (const auto &slot: m_getOutputSlots()) {
                auto slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
                if (vNode.getAttribute("lid") == ez::str::toStr("%u", idx++)) {
                    slot_ptr->setUuid(vNode.getAttribute<ez::Uuid>("gid"));
                }
            }
        }
    }
    RecursParsingConfigChilds(vNode, vUserDatas);
    return false;
}

const BaseStyle &BaseNode::getParentStyle() {
    return m_parentStyle;
}

BaseStyle BaseNode::getNodeStyle() {
    return m_nodeStyle;
}

BaseSlotWeak
BaseNode::findSlotByTypeAndOptionalName(ez::SlotDir vDir, const std::string &vType, const std::string &vName) {
    BaseSlotWeak ret;
    if (!vType.empty()) {
        if (vDir == ez::SlotDir::INPUT) {
            for (const auto &slot: m_getInputSlots()) {
                auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
                const auto &datas = base_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>();
                if (datas.type == vType) {
                    ret = base_slot_ptr;//we save, the ptr but we continue to search a name
                    if (datas.name == vName) {
                        break;// we found the name we stop it
                    }
                }
            }
        } else if (vDir == ez::SlotDir::OUTPUT) {
            for (const auto &slot: m_getOutputSlots()) {
                auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
                const auto &datas = base_slot_ptr->getDatas<BaseSlot::BaseSlotDatas>();
                if (datas.type == vType) {
                    ret = base_slot_ptr;  // we save, the ptr but we continue to search a name
                    if (datas.name == vName) {
                        break;  // we found the name we stop it
                    }
                }
            }
        }
    }
    return ret;
}

void BaseNode::setUuid(const ez::Uuid vUUID) {
    ez::UUID::setUuid(vUUID);
    m_nodeID = getUuid();
}

void BaseNode::beforeXmlLoading() {
    m_xmlLoading = true;
}

void BaseNode::afterXmlLoading() {
    m_xmlLoading = false;
}

//////////////////////////////////////////////////////////////////////////////
////// DRAW DEBUG INFOS //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::drawDebugInfos() {
    const auto &nodeDatas = getDatas<BaseNodeDatas>();
    ImGui::Text("Node : %s (%s)", nodeDatas.name.c_str(), nodeDatas.type.c_str());
    ImGui::Indent();
    for (const auto &slot: m_getInputSlots()) {
        auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
        if (base_slot_ptr != nullptr) {
            base_slot_ptr->drawDebugInfos();
        } else {
            ImGui::Text("Slot [IN] : [%s]", "Empty");
        }
    }

    for (const auto &slot: m_getOutputSlots()) {
        auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
        if (base_slot_ptr != nullptr) {
            base_slot_ptr->drawDebugInfos();
        } else {
            ImGui::Text("Slot [OUT] : [%s]", "Empty");
        }
    }
    ImGui::Unindent();
}

//////////////////////////////////////////////////////////////////////////////
////// DRAW //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool BaseNode::m_drawBegin() {
    nd::PushStyleVar(nd::StyleVar_NodePadding, ImVec4(4, 2, 4, 4));
    nd::BeginNode(m_nodeID);
    ImGui::PushID(this);
    ImGui::BeginVertical("node");
    return true;
}

bool BaseNode::m_drawHeader() {
    ImGui::BeginHorizontal("header");
    ImGui::Spring(1, 5.0f);
    const auto &datas = getDatas<BaseNodeDatas>();
    const bool pushed = ImGui::PushStyleColorWithContrast4(datas.color, ImGuiCol_Text,
                                                           ImGui::CustomStyle::puContrastedTextColor,
                                                           ImGui::CustomStyle::puContrastRatio);
    ImGui::TextUnformatted(getDatas<BaseNodeDatas>().name.c_str());
    if (pushed) {
        ImGui::PopStyleColor();
    }
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
    for (auto &slot: m_getInputSlotsRef()) {  // slots
        std::static_pointer_cast<BaseSlot>(slot.lock())->draw();
    }
    ImGui::EndVertical();
    ImGui::Spring(1, 5.0f);                               // pour que BeginVertical soi pouss� au bout
    ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);  // 1.0f pour que l'interieur soit align� sur la fin
    for (auto &slot: m_getOutputSlotsRef()) {            // slots
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
        auto draw_list_ptr = nd::GetNodeBackgroundDrawList(m_nodeID);
        if (draw_list_ptr) {
            ImGuiContext &g = *GImGui;
            const auto itemRect = g.LastItemData.Rect;
            if (m_headerRect.GetSize().y > 0.0f) {
                const ImVec4 NodePadding = nd::GetStyle().NodePadding;
                const auto halfBorderWidth = nd::GetStyle().NodeBorderWidth;
                draw_list_ptr->AddRectFilled(
                        m_headerRect.Min - ImVec2(NodePadding.x - halfBorderWidth, NodePadding.y - halfBorderWidth),
                        m_headerRect.Max + ImVec2(NodePadding.z - halfBorderWidth, 0),
                        getDatas<BaseNodeDatas>().color,
                        nd::GetStyle().NodeRounding,
                        ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight);
                auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);
                draw_list_ptr->AddLine(
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
    for (auto &slot: m_getInputSlotsRef()) {
        auto ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
        if (nd::GetHoveredPin().Get() == ptr->getUuid()) {
            ptr->m_drawHoveredSlotText(ptr->m_pos, false, 0, 0);
        }
    }

    for (auto &slot: m_getOutputSlotsRef()) {
        auto ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
        if (nd::GetHoveredPin().Get() == ptr->getUuid()) {
            ptr->m_drawHoveredSlotText(ptr->m_pos, false, 0, 0);
        }
    }

    m_displayInfosOnTopOfTheNode();

    return false;
}

void BaseNode::m_slotWasJustConnected(const BaseSlotWeak &vOwnNodeSlot, const BaseSlotWeak &vExternNodeSlot) {}

void BaseNode::m_slotWasJustDisConnected(const BaseSlotWeak &vOwnNodeSlot, const BaseSlotWeak &vExternNodeSlot) {}

void BaseNode::m_displayInfosOnTopOfTheNode() {
    if (getParentStyle().debugMode) {
        auto draw_list_ptr = ImGui::GetWindowDrawList();
        if (draw_list_ptr) {
            auto datas = getDatas<BaseNodeDatas>();
            const std::string &debugInfos = ez::str::toStr("Used(%s)\nCell(%i, %i)", datas.layout.used,
                                                           datas.layout.cell.x, datas.layout.cell.y);
            ImVec2 txtSize = ImGui::CalcTextSize(debugInfos.c_str());
            draw_list_ptr->AddText(m_pos - ImVec2(0, txtSize.y), ImGui::GetColorU32(ImGuiCol_Text), debugInfos.c_str());
        }
    }
}

BaseSlotWeak BaseNode::m_findSlotById(nd::PinId vId) {
    if (vId) {
        for (const auto &slot: m_getInputSlots()) {
            auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
            if (base_slot_ptr->m_pinID == vId) {
                return base_slot_ptr;
            }
        }
        for (const auto &slot: this->m_getOutputSlots()) {
            auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
            if (base_slot_ptr->m_pinID == vId) {
                return base_slot_ptr;
            }
        }
    }
    return {};
}


// will return all connected link from all slots
// for destruction by the graph
// since ther links are owned by the graph
BaseLinkWeakCnt BaseNode::m_getConnectedLinks() {
    BaseLinkWeakCnt ret;
    for (const auto &slot: m_getInputSlots()) {
        auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
        ret.tryMerge(base_slot_ptr->getLinks());
    }
    for (const auto &slot: m_getOutputSlots()) {
        auto base_slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
        ret.tryMerge(base_slot_ptr->getLinks());
    }
    return ret;
}

bool BaseNode::m_isXmlLoading() {
    return m_xmlLoading;
}

ez::RetCodes BaseNode::m_delSlot(const ez::SlotWeak &vSlot) {
    if (!vSlot.expired()) {
        auto graph_ptr = getParentGraph<BaseGraph>().lock();
        if (graph_ptr != nullptr) {
            auto slot_ptr = std::static_pointer_cast<BaseSlot>(vSlot.lock());
            graph_ptr->disconnectSlot(slot_ptr);
            return ez::Node::m_delSlot(vSlot);
        }
        return ez::RetCodes::FAILED_GRAPH_PTR_NULL;
    }
    return ez::RetCodes::FAILED_SLOT_PTR_NULL;
}
