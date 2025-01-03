#pragma once

#include <apis/ezRenamerPluginApi.h>

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>

#include "baseStyle.h"
#include "baseSlot.h"

#include <unordered_map>

#include <memory>

class BaseLink;
typedef std::shared_ptr<BaseLink> BaseLinkPtr;
typedef std::weak_ptr<BaseLink> BaseLinkWeak;

class BaseLink : public ez::UUID {
    friend class BaseGraph;

private:  // Style
    const BaseStyle& m_parentStyle;

private:
    BaseLinkWeak m_This;
    BaseSlotWeak m_in;
    BaseSlotWeak m_out;
    float m_thick = 2.0f;
    nd::LinkId m_linkId = 0;
    ImU32 m_color{IM_COL32(100, 100, 0, 200)};

public:  // Static
    static BaseLinkPtr create(const BaseStyle& vParentStyle, const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd);

public:
    explicit BaseLink(const BaseStyle& vParentStyle);
    ~BaseLink() override = default;
    bool init(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd);
    bool draw();
};
