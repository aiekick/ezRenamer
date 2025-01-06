#pragma once

#include <memory>
#include <ezlibs/ezCnt.hpp>
#include <ezlibs/ezGraph.hpp>

#include <ImGuiPack.h>

struct BaseStyle {
    bool debugMode = false;
    nd::Style style;
};

class BaseGraph;
typedef std::shared_ptr<BaseGraph> BaseGraphPtr;
typedef std::weak_ptr<BaseGraph> BaseGraphWeak;

class BaseLink;
typedef std::shared_ptr<BaseLink> BaseLinkPtr;
typedef std::weak_ptr<BaseLink> BaseLinkWeak;
typedef ez::cnt::DicoVector<ez::Uuid, BaseLinkWeak> BaseLinkWeakCnt;
typedef ez::cnt::DicoVector<ez::Uuid, BaseLinkPtr> BaseLinkPtrCnt;

class BaseNode;
typedef std::shared_ptr<BaseNode> BaseNodePtr;
typedef std::weak_ptr<BaseNode> BaseNodeWeak;

class BaseSlot;
typedef std::shared_ptr<BaseSlot> BaseSlotPtr;
typedef std::weak_ptr<BaseSlot> BaseSlotWeak;

class IDrawDebugInfos {
public:
    virtual void drawDebugInfos() = 0;
};
