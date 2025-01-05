#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include "baseStyle.h"
#include "baseNode.h"
#include "baseSlot.h"
#include "baseLink.h"
#include "baseLibrary.h"

#include <vector>
#include <functional>
#include <unordered_map>

class BaseGraph;
typedef std::shared_ptr<BaseGraph> BaseGraphPtr;
typedef std::weak_ptr<BaseGraph> BaseGraphWeak;

class BaseGraph  //
    : public ez::Graph,
      public ez::xml::Config,
      public rnm::GuiInterface,
      public rnm::NodeInterface {
public:
    struct BaseGraphDatas : public ez::GraphDatas {
        bool showFlow = false; // will display the flow of of link types
        std::string flowType; // the type for show the flow with F key
        ImGuiKey showFlowKey = ImGuiKey_Backspace; // the key who start the flow display
    };
    typedef std::function<void(const BaseGraphWeak&)> BgRightClickActionFunctor;
    typedef std::function<bool(const BaseGraphWeak&, const BaseSlotWeak&)> PrepareForCreateNodeFromSlotActionFunctor;
    typedef ez::Uuid LinkUuid;

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
    std::unordered_map<LinkUuid, BaseLinkPtr> m_links;  // linkId, link // for search query
    BgRightClickActionFunctor m_BgRightClickAction = nullptr;
    PrepareForCreateNodeFromSlotActionFunctor m_PrepareForCreateNodeFromSlotActionFunctor = nullptr;
    std::vector<nd::NodeId> m_nodesToCopy;  // for copy/paste
    ImVec2 m_nodesCopyOffset;

public:  // Normal
    template <typename T>
    explicit BaseGraph(const BaseStyle& vParentStyle, const T& vDatas)  //
        : m_parentStyle(vParentStyle), ez::Graph(std::make_shared<T>(vDatas)) {
        static_assert(std::is_base_of<BaseGraphDatas, T>::value, "T must derive of BaseGraphDatas");
        m_init();
    }
    ~BaseGraph() override { m_unit(); }

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

    bool connectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

    void setBgRightClickAction(const BgRightClickActionFunctor& vFunctor);
    void setPrepareForCreateNodeFromSlotActionFunctor(const PrepareForCreateNodeFromSlotActionFunctor& vFunctor);

public:  // Template
    template <typename T>
    std::shared_ptr<T> createChildNode() {
        static_assert(std::is_base_of<BaseNode, T>::value, "T must derive of BaseNode");
        auto node_ptr = std::make_shared<T>(m_parentStyle);
        node_ptr->m_setThis(node_ptr);
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

    template <typename T>
    std::shared_ptr<T> cloneChildNode(const std::weak_ptr<T>& vNodeToClone, const ImVec2& vNewPos) {
        static_assert(std::is_base_of<BaseNode, T>::value, "T must derive of BaseNode");
        auto node_ptr = vNodeToClone.lock();
        if (node_ptr != nullptr) {
            auto duplicated_node_ptr = node_ptr->clone();
            if (m_addNode(duplicated_node_ptr) != ez::RetCodes::SUCCESS) {
                duplicated_node_ptr.reset();
            } else {
                nd::SetNodePosition(duplicated_node_ptr->m_nodeID, m_openPopupPosition);
            }
            return duplicated_node_ptr;
        }
        return nullptr;
    }

private:  // Graph
    void m_init();
    void m_unit();
    void m_drawPopups();
    void m_drawBgContextMenuPopup();
    void m_drawLinks();

    void m_doCreateLinkOrNode();
    void m_doDeleteLinkOrNode();

    void m_doShorcutsOnNode();

    // create node with a specific slot type in mind
    void m_doCreateNodeFromSlot(const BaseSlotWeak& vSlot);
    bool m_PrepareForCreateNodeFromSlot(const BaseSlotWeak& vSlot);

    void m_copySelectedNodes();
    void m_pasteNodesAtMousePos();
    void m_duplicateSelectedNodes(const ImVec2& vOffset);
    void m_duplicateNode(uint32_t vNodeId, const ImVec2& vOffsetPos);

	// finders
    BaseNodeWeak m_findNode(nd::NodeId vId);
    BaseNodeWeak m_findNodeByName(const std::string& vName);
    BaseLinkWeak m_findLink(nd::LinkId vId);
    BaseSlotWeak m_findSlot(nd::PinId vId);

	bool m_addLink(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd);
    bool m_breakLink(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd);
    bool m_breakLink(const BaseLinkWeak& vLink);
    // will check each links, and will delete 
    // all links who are connected on one side only
    void m_delOneSideLinks();

    bool m_connectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);
    bool m_disconnectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);
    bool m_disconnectLink(const BaseLinkWeak& vLink);

    bool m_delNode(const BaseNodeWeak& vNode);
};
