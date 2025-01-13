#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include <graph/base/baseDefs.h>

#include <memory>
#include <unordered_map>

// to put in each derived nodes
#define ENABLE_CLONE(className)                    \
    BaseNodePtr clone() const override {           \
        return std::make_shared<className>(*this); \
    }

class BaseNode  //
    : public ez::Node,
      public ez::xml::Config,
      public rnm::GuiInterface,
      public rnm::NodeInterface,
      public IDrawDebugInfos {
    friend class BaseLayout;
    friend class BaseGraph;
    friend class BaseSlot;

public:
    struct BaseNodeDatas : public ez::NodeDatas {
        struct NodeLayout {
            bool used = false;               // utilise dans le code ou non
            bool hidden = false;             // visibilite du node
            bool inserted = false;           // pour voir si il y a des doublon dasn des colonnes
            bool rootUsed = false;           // ce node est le root
            bool graphDisabled = false;      // pas possible d'ouvrir ce graph
            bool deletionDisabled = false;   // pas possible d'effacer ce node
            ez::ivec2 cell = ez::ivec2(-1);  // layout x:column, y:row
        } layout;
        ImU32 color{IM_COL32(0, 100, 0, 200)};
        BaseNodeDatas() = default;
        BaseNodeDatas(const std::string& vName, const std::string& vType, ImU32 vColor = IM_COL32(0, 100, 0, 200)) : ez::NodeDatas(vName, vType), color(vColor) {}
    };

private:  // Style
    const BaseStyle& m_parentStyle;
    BaseStyle m_nodeStyle;

protected:  // Node
    ImVec2 m_pos{};
    ImVec2 m_size{};
    nd::NodeId m_nodeID{};

private:  // Node
    ImRect m_headerRect{};
    bool m_isHovered{false};
    bool m_isSelected{false};
    bool m_xmlLoading = false;

private:  // Graph
    nd::EditorContext* m_pNodeGraphContext{nullptr};
    std::set<ez::Uuid> m_selectedNodes{};

private:  // popups
    bool m_createNewNode{false};
    ImVec2 m_openPopupPosition{};
    nd::PinId m_contextMenuSlotId{};
    nd::NodeId m_contextMenuNodeId{};
    nd::LinkId m_contextMenuLinkId{};

public:  // Normal
    template <typename T>
    explicit BaseNode(const BaseStyle& vParentStyle, const T& vDatas) : m_parentStyle(vParentStyle), ez::Node(vDatas) {
        static_assert(std::is_base_of<BaseNodeDatas, T>::value, "T must derive of BaseNodeDatas");
    }
    ~BaseNode() override = default;

    bool init() override;

    bool drawWidgets(const uint32_t& vFrame) override;
    bool drawNodeWidget(const uint32_t& vFrame) override;

    bool drawNode();

    // for the ablity to duplicate a node in graph
    virtual BaseNodePtr clone() const = 0;

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

    const BaseStyle& getParentStyle();
    BaseStyle getNodeStyle();

    virtual BaseSlotWeak findSlotByTypeAndOptionalName(ez::SlotDir vDir, const std::string& vType, const std::string& vName);

    void drawDebugInfos() override;

    void setUuid(const ez::Uuid vUUID) override;

    void beforeXmlLoading() override;
    void afterXmlLoading() override;

public:  // Template
    template <typename T>
    std::weak_ptr<T> createChildSlot() {
        static_assert(std::is_base_of<BaseSlot, T>::value, "T must derive of BaseSlot");
        auto slot_ptr = std::make_shared<T>(m_parentStyle);
        slot_ptr->m_setThis(slot_ptr);
        slot_ptr->setUuid(slot_ptr->getUuid());  // call the virtual setUuid for derived classes
        if (!slot_ptr->init()) {
            slot_ptr.reset();
        } else {
            if (m_addSlot(slot_ptr) != ez::RetCodes::SUCCESS) {
                slot_ptr.reset();
            }
        }
        return slot_ptr;
    }

protected:  // Node
    virtual bool m_drawBegin();
    virtual bool m_drawHeader();
    virtual bool m_drawContent();
    virtual bool m_drawInputSlots();
    virtual bool m_drawOutputSlots();
    virtual bool m_drawFooter();
    virtual bool m_drawEnd();
    virtual bool m_drawHints();
    virtual void m_slotWasJustConnected(const BaseSlotWeak& vOwnNodeSlot, const BaseSlotWeak& vExternNodeSlot); // called by vOwnNodeSlot on connection
    virtual void m_slotWasJustDisConnected(const BaseSlotWeak& vOwnNodeSlot, const BaseSlotWeak& vExternNodeSlot);  // called by vOwnNodeSlot on dicconnection
    virtual void m_displayInfosOnTopOfTheNode();
    virtual BaseSlotWeak m_findSlotById(nd::PinId vId);
    virtual BaseLinkWeakCnt m_getConnectedLinks();
    bool m_isXmlLoading();
};

