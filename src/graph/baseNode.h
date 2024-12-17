#pragma once

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <graph/baseCanvas.h>
#include <graph/baseSlot.h>
#include <unordered_map>

class BaseNode;
typedef std::shared_ptr<BaseNode> BaseNodePtr;
typedef std::weak_ptr<BaseNode> BaseNodeWeak;

class BaseNode : public ez::Node {
public:
    struct NodeConfig : public ez::NodeDatas {
        float cornerRadius = 2.5f;
        float borderThickness = -1.35f;
        float borderHoveredThickness = 2.0f;
        float borderSelectedThickness = 2.0f;
        ImU32 bgColor = IM_COL32(55, 64, 75, 255);
        ImU32 borderColor = IM_COL32(30, 38, 41, 140);
        ImU32 headerBgColor = IM_COL32(191, 90, 90, 255);
        ImVec4 padding = ImVec4(4.0f, 0.0f, 4.0f, 0.0f); // TLBR
        ImColor headerTitleColor = ImColor(233, 241, 244, 255);
        ImU32 borderHoveredColor = IM_COL32(170, 190, 205, 230);
        ImU32 borderSelectedColor = IM_COL32(170, 190, 205, 230);
    };

public:  // Static
    static BaseNodePtr create(const NodeConfig& vNodeDatas) {
        auto node_ptr = std::make_shared<BaseNode>(vNodeDatas);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        }
        return node_ptr;
    }

private: // Node
    ImVec2 m_pos;
    ImVec2 m_size;
    NodeConfig m_nodeConfig;
    std::string m_nodeTitle{"Node"};
    ImU32 m_nodeHeaderColor{IM_COL32(100, 0, 0, 200)};
    bool m_isHovered = false;
    bool m_isSelected = false;

private: // Graph
    BaseCanvas m_canvas;
    ImCanvas::Config m_canvasConfig;
    ImCanvas::GridConfig m_gridConfig;
    std::set<ez::Uuid> m_selectedNodes;

public: // Template
    template <typename U, typename = std::enable_if<std::is_base_of<ez::Node, U>::value>>
    std::weak_ptr<U> createChildNode(const NodeConfig& vNodeDatas) {
        auto node_ptr = std::make_shared<U>(vNodeDatas);
        if (!node_ptr->init()) {
            node_ptr.reset();
        } else {
            if (m_addNode(node_ptr) != ez::RetCodes::SUCCESS) {
                node_ptr.reset();
            }
        }
        return node_ptr;
    }

    template <typename U, typename = std::enable_if<std::is_base_of<ez::Slot, U>::value>>
    std::weak_ptr<U> addSlot(const BaseSlot::SlotConfig& vSlotDatas) {
        auto slot_ptr = std::make_shared<U>(vSlotDatas);
        if (!slot_ptr->init()) {
            slot_ptr.reset();
        } else {
            if (m_addSlot(slot_ptr) != ez::RetCodes::SUCCESS) {
                slot_ptr.reset();
            }
        }
        return slot_ptr;
    }

public: // Normal
    template <typename T, typename = std::enable_if<std::is_base_of<NodeConfig, T>::value>>
    explicit BaseNode(const T& vDatas) : ez::Node(std::make_shared<T>(vDatas)) {}

    bool drawGraph();
    bool drawNode();
    ez::RetCodes connectSlots(ez::SlotWeak vFrom, ez::SlotWeak vTo);

private: // Node
    bool m_drawNodeHeader();
    bool m_drawNodeContent();
    bool m_drawNodeInputSlots();
    bool m_drawNodeOutputSlots();

private: // Graph
    BaseNodeWeak m_getParentGraph();
    BaseNodePtr m_getParentGraphPtr();
    bool m_drawGraphNodes();
};
