#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include "baseStyle.h"
#include "baseNode.h"
#include "baseSlot.h"

#include <functional>
#include <unordered_map>

class BaseLink;
typedef std::shared_ptr<BaseLink> BaseLinkPtr;
typedef std::weak_ptr<BaseLink> BaseLinkWeak;

class BaseLink {
    friend class BaseGraph;

private:
    BaseSlotWeak m_in;
    BaseSlotWeak m_out;
    nd::LinkId m_linkId = 0;
    ImColor m_color = ImColor(255, 255, 0, 255);
    float m_thick = 2.0f;
};

class BaseGraph;
typedef std::shared_ptr<BaseGraph> BaseGraphPtr;
typedef std::weak_ptr<BaseGraph> BaseGraphWeak;

class BaseGraph  //
    : public ez::Graph,
      public ez::xml::Config,
      public rnm::GuiInterface,
      public rnm::NodeInterface {
public:
    struct BaseGraphDatas : public ez::GraphDatas {};
    typedef std::function<void(const BaseGraphWeak& vGraph)> BasicActionFunctor;

public:  // Static
    static BaseGraphPtr create(const BaseStyle& vParentStyle, const BaseGraphDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<BaseGraph>(vParentStyle, vNodeDatas);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        }
        return node_ptr;
    }

private:  // Common
    const BaseStyle& m_parentStyle;

private:  // Graph
    nd::EditorContext* m_pCanvas{nullptr};
    ImVec2 m_openPopupPosition;
    nd::NodeId m_contextMenuNodeId = 0;
    nd::PinId m_contextMenuSlotId = 0;
    nd::LinkId m_contextMenuLinkId = 0;
    std::unordered_map<ez::Uuid, BaseLinkPtr> m_links;  // linkId, link // for search query
    BasicActionFunctor m_bgRightClickAction = nullptr;

public:  // Normal
    template <typename T, typename = std::enable_if<std::is_base_of<BaseGraphDatas, T>::value>>
    explicit BaseGraph(const BaseStyle& vParentStyle, const T& vDatas)  //
        : m_parentStyle(vParentStyle), ez::Graph(std::make_shared<T>(vDatas)) {
        m_init();
    }
    ~BaseGraph() { m_unit(); }

    bool drawGraph();

    bool drawNodeWidget(const uint32_t& vFrame) override;
    bool drawWidgets(const uint32_t& vFrame) override;

    void zoomToContent() const;
    void navigateToContent() const;
    void zoomToSelection() const;
    void navigateToSelection() const;

    ImVec2 getMousePos() const;

    ImVec2 getCanvasOffset() const;
    float getCanvasScale() const;
    void setCanvasOffset(const ImVec2& vOffset);
    void setCanvasScale(const float& vScale);

    void copySelectedNodes();
    void pasteNodesAtMousePos();
    void duplicateSelectedNodes(ImVec2 vOffset);

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

    void setBgRightClickAction(const BasicActionFunctor& vFunctor);

public:  // Template
    template <typename U, typename = std::enable_if<std::is_base_of<BaseNode, U>::value>>
    std::shared_ptr<U> createChildNode(const BaseNode::BaseNodeDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<U>(m_parentStyle, vNodeDatas);
        if (!node_ptr->init()) {
            node_ptr.reset();
        } else {
            if (m_addNode(node_ptr) != ez::RetCodes::SUCCESS) {
                node_ptr.reset();
            } else {
                nd::SetNodePosition(node_ptr->m_nodeID, m_openPopupPosition);
            }
        }
        return node_ptr;
    }

    template <typename U, typename = std::enable_if<std::is_base_of<BaseNode, U>::value>>
    std::shared_ptr<U> createChildNode() {
        auto node_ptr = std::make_shared<U>(m_parentStyle);
        if (!node_ptr->init()) {
            node_ptr.reset();
        } else {
            if (m_addNode(node_ptr) != ez::RetCodes::SUCCESS) {
                node_ptr.reset();
            } else {
                nd::SetNodePosition(node_ptr->m_nodeID, m_openPopupPosition);
            }
        }
        return node_ptr;
    }

private:  // Graph
    void m_init();
    void m_unit();
    void m_drawPopups();
    void m_drawBgContextMenuPopup();
    void m_drawLinks();
};
