#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include <graph/baseStyle.h>
#include <graph/baseNode.h>
#include <graph/baseSlot.h>

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
    struct BaseGraphDatas : public ez::GraphDatas {};

public:  // Static
    static BaseGraphPtr create(const BaseStyle& vStyle, const BaseGraphDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<BaseGraph>(vStyle, vNodeDatas);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        }
        return node_ptr;
    }

private:  // Common
    const BaseStyle& m_baseStyle;

private: // Graph
    nd::EditorContext* m_pCanvas{nullptr};

public: // Normal
    template <typename T, typename = std::enable_if<std::is_base_of<BaseGraphDatas, T>::value>>
    explicit BaseGraph(const BaseStyle& vStyle, const T& vDatas) //
        : m_baseStyle(vStyle), ez::Graph(std::make_shared<T>(vDatas)) {
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

public:  // Template
    template <typename U, typename = std::enable_if<std::is_base_of<ez::Node, U>::value>>
    std::weak_ptr<U> createChildNode(const BaseStyle& vStyle, const BaseNode::BaseNodeDatas& vNodeDatas) {
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

private: // Graph
    void m_init();
    void m_unit();

};

