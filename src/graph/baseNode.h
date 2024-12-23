#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include <graph/baseStyle.h>
#include <graph/baseSlot.h>

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

public:
    struct BaseNodeDatas : public ez::NodeDatas {
        struct NodeLayout {
            bool used = false;                  // utilise dans le code ou non
            bool hidden = false;                // visibilite du node
            ez::ivec2 cell = ez::ivec2(-1);     // layout x:column, y:row
            bool inserted = false;              // pour voir si il y a des doublon dasn des colonnes
            bool rootUsed = false;              // ce node est le root
            bool graphDisabled = false;         // pas possible d'ouvrir ce graph
            bool deletionDisabled = false;      // pas possible d'effacer ce node
            bool changed = false;               // need to save
        } layout;
    };

public:  // Static
    static BaseNodePtr create(const BaseStyle& vStyle, const BaseNodeDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<BaseNode>(vStyle, vNodeDatas);
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

private:  // Common
    const BaseStyle& m_baseStyle;

private:  // Node
    ImVec2 m_pos{};
    ImVec2 m_size{};
    nd::NodeId nodeID{};
    BaseNodeDatas m_nodeConfig{};
    std::string m_nodeTitle{"Node"};
    ImU32 m_nodeHeaderColor{IM_COL32(100, 0, 0, 200)};
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
    template <typename T, typename = std::enable_if<std::is_base_of<BaseNodeDatas, T>::value>>
    explicit BaseNode(const BaseStyle& vStyle, const T& vDatas) : m_baseStyle(vStyle), ez::Node(vDatas) {}

    bool drawWidgets(const uint32_t& vFrame) override;
    bool drawNodeWidget(const uint32_t& vFrame) override;

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;


public:  // Template
    template <typename U, typename = std::enable_if<std::is_base_of<ez::Node, U>::value>>
    std::weak_ptr<U> createChildNode(const BaseStyle& vStyle, const BaseNodeDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<U>(vStyle, vNodeDatas);
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
    std::weak_ptr<U> addSlot(const BaseStyle& vStyle, const BaseSlot::BaseSlotDatas& vSlotDatas) {
        auto slot_ptr = std::make_shared<U>(vStyle, vSlotDatas);
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
    bool m_drawNodeHeader();
    bool m_drawNodeContent();
    bool m_drawNodeInputSlots();
    bool m_drawNodeOutputSlots();
    bool m_drawBegin();
    bool m_drawHeader();
    bool m_drawFooter();
    bool m_drawEnd();
    void m_displayInfosOnTopOfTheNode();
};

