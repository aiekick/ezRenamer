#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>

#include <ezlibs/ezXmlConfig.hpp>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezCnt.hpp>

#include <graph/base/baseDefs.h>

#include <unordered_map>

#include <memory>

class BaseLink : public ez::UUID, public IDrawDebugInfos, public ez::xml::Config {
    friend class BaseGraph;

private:  // Style
    const BaseStyle& m_parentStyle;

private:
    BaseLinkWeak m_This;
    BaseSlotWeak m_in;
    BaseSlotWeak m_out;
    float m_thick = 2.0f;
    nd::LinkId m_linkId = 0;
    std::string m_type;                         // will retain the type of one of the slots
    ImU32 m_color{IM_COL32(100, 100, 0, 200)};  // will retain the color of one of the slots

public:  // Static
    static BaseLinkPtr create(const BaseStyle& vParentStyle, const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd);

public:
    explicit BaseLink(const BaseStyle& vParentStyle);
    ~BaseLink() override = default;
    bool init(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd);
    bool draw();
    const BaseSlotWeak& getInSlot() const;
    const BaseSlotWeak& getOutSlot() const;
    
    void drawDebugInfos() override;

     ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
     bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;
};
