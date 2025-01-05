#pragma once

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>
#include <ezlibs/ezCnt.hpp>

#include "baseStyle.h"
#include "baseSlot.h"
#include "baseLink.h"

#include "interfaces/SlotColorBankInterface.h"

#include <unordered_map>
#include <map>

class BaseSlot;
typedef std::shared_ptr<BaseSlot> BaseSlotPtr;
typedef std::weak_ptr<BaseSlot> BaseSlotWeak;

class BaseSlot  //
    : public ez::Slot,
      public ez::xml::Config {
    friend class BaseGraph;
    friend class BaseNode;
    friend class BaseLink;

public:
    struct BaseSlotDatas : ez::SlotDatas {
        float radius{2.5f};
        ImU32 color{IM_COL32(200, 200, 0, 255)};
        ImU32 hovered_color{IM_COL32(0, 200, 0, 255)};
        ImU32 inner_color{IM_COL32(0, 200, 0, 255)};
        ImVec2 slotPadding{0, 0};
        float slotIconSize{16.0f};
        bool highLighted{false};
        bool showWidget{false};
        bool hideName{false};
        bool connected{false};
        bool hidden{false};
        bool debugMode{false};
        BaseSlotDatas() = default;
        BaseSlotDatas(
            const std::string& vName,
            const std::string& vType,
            const ez::SlotDir vSlotDir,
            const SlotColorBankInterface* vSlotColorBankPtr = nullptr,
            ez::UserDatas vUserDatas = nullptr)
            : ez::SlotDatas(vName, vType, vSlotDir, vUserDatas) {
            if (vSlotColorBankPtr != nullptr) {
                if (vSlotColorBankPtr->getSlotColor(vType, color)) {
                    hovered_color = color;
                    inner_color = color;
                }
            }
        }
    };

public:  // Static
    template<typename T>
    static std::shared_ptr<T> create(const BaseStyle& vParentStyle, const BaseSlotDatas& vSlotDatas) {
        static_assert(std::is_base_of<BaseSlot, T>::value, "T must derive of BaseSlot");
        auto slot_ptr = std::make_shared<T>(vParentStyle, vSlotDatas);
        slot_ptr->m_setThis(slot_ptr);
        if (!slot_ptr->init()) {
            slot_ptr.reset();
        }
        return slot_ptr;
    }

private:  // Common
    const BaseStyle& m_parentStyle;

private:
    ImVec2 m_pos;
    ImVec2 m_size;
    nd::PinId m_pinID = 0;

    // un slot peut etre attaché a plusieurs autre slots, 
    // donc plusieurs links
    ez::cnt::DicoVector<ez::Uuid, BaseLinkWeak> m_links;

public:
    template <typename T>
    explicit BaseSlot(const BaseStyle& vParentStyle, const T& vDatas)
        : m_parentStyle(vParentStyle), ez::Slot(std::make_shared<T>(vDatas)) {
        static_assert(std::is_base_of<BaseSlotDatas, T>::value, "T must derive of BaseSlotDatas");
    }
    ~BaseSlot() override = default;

    bool init() override {
        if (ez::Slot::init()) {
            m_pinID = getUuid();
            return true;
        }
        return false;
    }

    void unit() override { EZ_TOOLS_DEBUG_BREAK; }

    void setRadius(const float vRadius);
    void setColor(const ImVec4& vColor);
    void setPos(const ImVec2& vPos);
    bool draw();

    bool isAnInput();
    bool isAnOutput();

    /// called when a slot was double clicked with mouse
    //virtual void mouseDoubleClickedOnSlot(const ImGuiMouseButton& vMouseButton);

    /// will remove the slot from the list linekdSLots
    //bool removeConnectedSlot(const BaseSlotWeak& vOtherSlot);

    void drawDebugInfos();

     ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
     bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

private:
    void m_drawInputWidget();
    void m_drawOutputWidget();

protected:
    void m_drawSlot();
    virtual void m_drawBaseSlot(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor);
    virtual void m_drawSlotText(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor);
};