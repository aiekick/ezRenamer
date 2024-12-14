#pragma once

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp> 

class BaseSlot;
typedef std::shared_ptr<BaseSlot> BaseSlotPtr;
typedef std::weak_ptr<BaseSlot> BaseSlotWeak;

class BaseSlot : public ez::Node {
public: // Static
    static BaseSlotPtr create(const ez::NodeDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<BaseSlot>();
        if (!node_ptr->initNode(vNodeDatas, node_ptr)) {
            node_ptr.reset();
        }
        return node_ptr;
    }

public: // Template
    template <typename U>
    BaseSlotWeak createChildNode(const ez::NodeDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<U>();
        if (!node_ptr->initNode(vNodeDatas, node_ptr)) {
            node_ptr.reset();
        } else {
            if (m_addNode(node_ptr) != ez::RetCodes::SUCCESS) {
                node_ptr.reset();
            }
        }
        return node_ptr;
    }
};
