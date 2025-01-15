#include "baseLink.h"

#include <graph/base/baseSlot.h>

BaseLinkPtr BaseLink::create(const BaseStyle& vParentStyle, const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd) {
    auto link_ptr = std::make_shared<BaseLink>(vParentStyle);
    link_ptr->m_This = link_ptr;
    if (!link_ptr->init(vStart, vEnd)) {
        link_ptr.reset();
    }
    return link_ptr;
}

BaseLink::BaseLink(const BaseStyle& vParentStyle) //
    : ez::UUID(this), m_parentStyle(vParentStyle) {}

bool BaseLink::init(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd) {
    bool ret = false;
    auto inPtr = vStart.lock();
    auto outPtr = vEnd.lock();
    if (inPtr != nullptr && outPtr != nullptr) {
        m_in = vStart;
        m_out = vEnd;
        m_linkId = getUuid();
        m_type = inPtr->getDatas<BaseSlot::BaseSlotDatas>().type;
        m_color = inPtr->getDatas<BaseSlot::BaseSlotDatas>().color;
        m_thick = 2.0f;
        ret = true;
    }
    return ret;
}

bool BaseLink::draw() {
    bool ret = false;
    auto inPtr = m_in.lock();
    auto outPtr = m_out.lock();
    if (inPtr != nullptr && outPtr != nullptr) {
        ret = nd::Link(m_linkId, inPtr->m_pinID, outPtr->m_pinID, m_color, m_thick);
    }
    return ret;
}

const BaseSlotWeak& BaseLink::getInSlot() const {
    return m_in;
}

const BaseSlotWeak& BaseLink::getOutSlot() const {
    return m_out;
}

//////////////////////////////////////////////////////////////////////////////
////// DRAW DEBUG INFOS //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseLink::drawDebugInfos() {
    ImGui::Text(
        "Link : %s -> %s",  //
        m_in.lock()->getDatas<BaseSlot::BaseSlotDatas>().name.c_str(),
        m_out.lock()->getDatas<BaseSlot::BaseSlotDatas>().name.c_str());
}

//////////////////////////////////////////////////////////////////////////////
////// CONFIGURATION /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

ez::xml::Nodes BaseLink::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node xml;
    xml.addChild("link").addAttribute("in", m_in.lock()->getUuid()).addAttribute("out", m_out.lock()->getUuid());
    return xml.getChildren();
}

bool BaseLink::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    return true;
}
