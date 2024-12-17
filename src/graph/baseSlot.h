#pragma once

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp> 

class BaseSlot;
typedef std::shared_ptr<BaseSlot> BaseSlotPtr;
typedef std::weak_ptr<BaseSlot> BaseSlotWeak;

class BaseSlot : public ez::Slot {
public:
    struct SlotConfig : ez::SlotDatas {
        float radius = 5.0f;
        ImU32 color = IM_COL32(200, 200, 0, 255);
        ImU32 hovered_color = IM_COL32(0, 200, 0, 255);
        SlotConfig() = default;
        SlotConfig(const std::string& vName, const std::string& vType, const ez::SlotDir vSlotDir, ez::UserDatas vUserDatas = nullptr)
            : ez::SlotDatas(vName, vType, vSlotDir, vUserDatas) {}
    };

public:  // Static
    static BaseSlotPtr create(const SlotConfig& vSlotDatas) {
        auto slot_ptr = std::make_shared<BaseSlot>(vSlotDatas);
        slot_ptr->m_setThis(slot_ptr);
        if (!slot_ptr->init()) {
            slot_ptr.reset();
        }
        return slot_ptr;
    }

private:
    ImVec2 m_pos;
    ImVec2 m_size;

public:
    template <typename T, typename = std::enable_if<std::is_base_of<SlotConfig, T>::value>>
    explicit BaseSlot(const T& vDatas) : ez::Slot(std::make_shared<T>(vDatas)) {}

    void setRadius(const float vRadius);
    void setColor(const ImVec4& vColor);
    void setPos(const ImVec2& vPos);
    bool draw();

private:
    bool m_drawSlot(const ImVec2& vPos);
};
