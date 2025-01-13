#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include <graph/base/baseDefs.h>

#include <vector>
#include <functional>
#include <unordered_map>

class BaseGraph  //
    : public ez::Graph,
      public ez::xml::Config,
      public rnm::GuiInterface,
      public rnm::NodeInterface,
      public IDrawDebugInfos {
public:
    struct BaseGraphDatas : public ez::GraphDatas {
        bool showFlow = false;                      // will display the flow of of link types
        std::string flowType;                       // the type for show the flow with F key
        ImGuiKey showFlowKey = ImGuiKey_Backspace;  // the key who start the flow display
    };
    typedef std::function<void(const BaseGraphWeak&)> BgRightClickActionFunctor;
    typedef std::function<bool(const BaseGraphWeak&, const BaseSlotWeak&)> PrepareForCreateNodeFromSlotActionFunctor;
    typedef std::function<bool(const BaseGraphWeak&, const ez::xml::Node&, const ez::xml::Node&)> LoadNodeFromXmlFunctor;
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
    nd::NodeId m_contextMenuNodeId{};
    nd::PinId m_contextMenuSlotId{};
    nd::LinkId m_contextMenuLinkId{};
    BaseLinkPtrCnt m_links;  // linkId, link // for search query
    LoadNodeFromXmlFunctor m_LoadNodeFromXmlFunctor{nullptr};
    BgRightClickActionFunctor m_BgRightClickActionFunctor{nullptr};
    PrepareForCreateNodeFromSlotActionFunctor m_PrepareForCreateNodeFromSlotActionFunctor{nullptr};
    std::vector<nd::NodeId> m_nodesToCopy;  // for copy/paste
    ImVec2 m_nodesCopyOffset;
    bool m_graphChanged{false};
    bool m_xmlLoading = false;

public:  // Normal
    template <typename T>
    explicit BaseGraph(const BaseStyle& vParentStyle, const T& vDatas)  //
        : m_parentStyle(vParentStyle), ez::Graph(std::make_shared<T>(vDatas)) {
        static_assert(std::is_base_of<BaseGraphDatas, T>::value, "T must derive of BaseGraphDatas");
    }
    ~BaseGraph() override;

    bool init() override;
    void unit() override;

    void clear() override;

    void setCurrentEditor() const;

    bool drawGraph();

    bool drawNodeWidget(const uint32_t& vFrame) override;
    bool drawWidgets(const uint32_t& vFrame) override;

    void zoomToContent() const;
    void navigateToContent() const;
    void zoomToSelection() const;
    void navigateToSelection() const;

    void setGraphChanged(bool vFlag);
    bool isGraphChanged() const;

    ImVec2 getMousePos() const;

    bool connectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);
    bool disconnectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);
    bool disconnectLink(const BaseLinkWeak& vLink);

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

    void setLoadNodeFromXmlFunctor(const LoadNodeFromXmlFunctor& vFunctor);
    void setBgRightClickActionFunctor(const BgRightClickActionFunctor& vFunctor);
    void setPrepareForCreateNodeFromSlotActionFunctor(const PrepareForCreateNodeFromSlotActionFunctor& vFunctor);

    void drawDebugInfos() override;

    void beforeXmlLoading() override;
    void afterXmlLoading() override;

public:  // Template
    template <typename T>
    std::weak_ptr<T> createChildNode() {
        static_assert(std::is_base_of<BaseNode, T>::value, "T must derive of BaseNode");
        auto node_ptr = std::make_shared<T>(m_parentStyle);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        } else {
            if (m_addNode(node_ptr) != ez::RetCodes::SUCCESS) {
                node_ptr.reset();
            } else {
                setCurrentEditor();
                nd::SetNodePosition(node_ptr->m_nodeID, m_openPopupPosition);
            }
        }
        return node_ptr;
    }

    template <typename T>
    std::weak_ptr<T> cloneChildNode(const std::weak_ptr<T>& vNodeToClone, const ImVec2& vNewPos) {
        static_assert(std::is_base_of<BaseNode, T>::value, "T must derive of BaseNode");
        auto node_ptr = vNodeToClone.lock();
        if (node_ptr != nullptr) {
            auto duplicated_node_ptr = node_ptr->clone();
            if (m_addNode(duplicated_node_ptr) != ez::RetCodes::SUCCESS) {
                duplicated_node_ptr.reset();
            } else {
                setCurrentEditor();
                nd::SetNodePosition(duplicated_node_ptr->m_nodeID, m_openPopupPosition);
            }
            return duplicated_node_ptr;
        }
        return {};
    }

private:  // Graph
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
    BaseSlotWeak m_findSlotById(nd::PinId vId);

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
