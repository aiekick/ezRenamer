﻿#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include "baseStyle.h"
#include "baseSlot.h"

#include <unordered_map>

class BaseNode;
typedef std::shared_ptr<BaseNode> BaseNodePtr;
typedef std::weak_ptr<BaseNode> BaseNodeWeak;

class BaseNode  //
    : public ez::Node,
      public ez::xml::Config,
      public rnm::GuiInterface,
      public rnm::TaskInterface,
      public rnm::NodeInterface { 
    friend class BaseGraph;

public:
    struct BaseNodeDatas : public ez::NodeDatas {
        struct NodeLayout {
            bool used = false;                  // utilise dans le code ou non
            bool hidden = false;                // visibilite du node
            bool inserted = false;              // pour voir si il y a des doublon dasn des colonnes
            bool rootUsed = false;              // ce node est le root
            bool graphDisabled = false;         // pas possible d'ouvrir ce graph
            bool deletionDisabled = false;      // pas possible d'effacer ce node
            ez::ivec2 cell = ez::ivec2(-1);     // layout x:column, y:row
        } layout;
        ImU32 color{IM_COL32(0, 100, 0, 200)};
        BaseNodeDatas() = default;
        BaseNodeDatas(const std::string& vName, const std::string& vType, ImU32 vColor = IM_COL32(0, 100, 0, 200)) : ez::NodeDatas(vName, vType), color(vColor) {}
    };

public:  // Static
    static BaseNodePtr create(const BaseStyle& vParentStyle, const BaseNodeDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<BaseNode>(vParentStyle, vNodeDatas);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        }
        return node_ptr;
    }

    static uint32_t GetFreeId() {
        static uint32_t freeId = 4577;
        return ++freeId;
    }

private:  // Style
    const BaseStyle& m_parentStyle;
    BaseStyle m_nodeStyle;

private:  // Node
    ImVec2 m_pos{};
    ImVec2 m_size{};
    nd::NodeId m_nodeID{};
    ImRect m_headerRect{};
    bool m_isHovered{false};
    bool m_isSelected{false};

private: // Graph
    nd::EditorContext* m_pNodeGraphContext{nullptr};
    std::set<ez::Uuid> m_selectedNodes{};

private:  // popups
    bool m_createNewNode{false};
    ImVec2 m_openPopupPosition{};
    nd::PinId m_contextMenuSlotId{};
    nd::NodeId m_contextMenuNodeId{};
    nd::LinkId m_contextMenuLinkId{};

public: // Normal
    template <typename T>
    explicit BaseNode(const BaseStyle& vParentStyle, const T& vDatas) : m_parentStyle(vParentStyle), ez::Node(vDatas) {
        static_assert(std::is_base_of<BaseNodeDatas, T>::value, "T must derive of BaseNodeDatas");
    }
    ~BaseNode() override = default;

    bool init() override { 
        if (ez::Node::init()) {
            m_nodeID = getUuid(); 
            return true;
        }
        return false;
    }

    bool drawWidgets(const uint32_t& vFrame) override;
    bool drawNodeWidget(const uint32_t& vFrame) override;

    bool drawNode();

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

    const BaseStyle& getParentStyle();
    BaseStyle getNodeStyle();

public:  // Template
    template <typename T>
    std::weak_ptr<T> createChildSlot() {
        static_assert(std::is_base_of<BaseSlot, T>::value, "T must derive of BaseSlot");
        auto slot_ptr = std::make_shared<T>(m_parentStyle);
        if (!slot_ptr->init()) {
            slot_ptr.reset();
        } else {
            if (m_addSlot(slot_ptr) != ez::RetCodes::SUCCESS) {
                slot_ptr.reset();
            }
        }
        return slot_ptr;
    }

private: // Node
    bool m_drawBegin();
    bool m_drawHeader();
    bool m_drawContent();
    bool m_drawInputSlots();
    bool m_drawOutputSlots();
    bool m_drawFooter();
    bool m_drawEnd();
    void m_displayInfosOnTopOfTheNode();
    BaseSlotWeak m_findSlot(nd::PinId vId);
};

