#pragma once

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp> 

class BaseSlot;
typedef std::shared_ptr<BaseSlot> BaseSlotPtr;
typedef std::weak_ptr<BaseSlot> BaseSlotWeak;

class BaseSlot : public ez::Slot {
private:
    ImVec2 m_pos;

public:
     void setPos(const ImVec2& vPos);
    bool draw();
};
