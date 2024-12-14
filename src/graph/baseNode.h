#pragma once

#include <imguipack/ImGuiPack.h>
#include <ezlibs/ezGraph.hpp>

class BaseNode;
typedef std::shared_ptr<BaseNode> BaseNodePtr;
typedef std::weak_ptr<BaseNode> BaseNodeWeak;

class BaseNode : public ez::Node {
public: // Static
    static BaseNodePtr create(const ez::NodeDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<BaseNode>();
        if (!node_ptr->initNode(vNodeDatas, node_ptr)) {
            node_ptr.reset();
        }
        return node_ptr;
    }

public: // Template
    template <typename U, typename = std::enable_if<std::is_base_of<ez::Node, U>::value>>
    std::weak_ptr<U> createChildNode(const ez::NodeDatas& vNodeDatas) {
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
    template <typename U, typename = std::enable_if<std::is_base_of<ez::Slot, U>::value>>
    std::weak_ptr<U> addSlot(const ez::SlotDatas& vSlotDatas) {
        auto slot_ptr = std::make_shared<U>();
        if (!slot_ptr->initSlot(vSlotDatas, slot_ptr)) {
            slot_ptr.reset();
        } else {
            if (m_addSlot(slot_ptr) != ez::RetCodes::SUCCESS) {
                slot_ptr.reset();
            }
        }
        return slot_ptr;
    }

public: // Normal
    ez::RetCodes connectSlots(ez::SlotWeak vFrom, ez::SlotWeak vTo) { return m_connectSlots(vFrom, vTo); }
};