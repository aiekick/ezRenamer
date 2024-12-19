#include <graph/nodeManager.h>
#include <graph/baseSlot.h>

std::unique_ptr<NodeManager> NodeManager::m_singleton = nullptr;

NodeManager* NodeManager::instance() {
    assert(m_singleton != nullptr);
    return m_singleton.get();
}

bool NodeManager::initInstance() {
    m_singleton = std::make_unique<NodeManager>();
    return instance()->init();
}

void NodeManager::unitInstance() {
    instance()->unit();
    m_singleton.reset();
}

bool NodeManager::init() {
    addSlotColor("NONE", ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    addSlotColor("FILES", ImVec4(0.5f, 0.5f, 0.9f, 1.0f));
    addSlotColor("TOKEN", ImVec4(0.9f, 0.9f, 0.1f, 1.0f));
    m_graphPtr = BaseNode::create({});
    auto node = m_graphPtr->createChildNode<BaseNode>({});
    auto ptr = node.lock();
    if (ptr != nullptr) {
        ptr->addSlot<BaseSlot>(BaseSlot::SlotConfig("in0", "base", ez::SlotDir::INPUT));
        ptr->addSlot<BaseSlot>(BaseSlot::SlotConfig("out0", "base", ez::SlotDir::OUTPUT));
    }
    return true;
}

void NodeManager::unit() {
    m_graphPtr.reset();
}

bool NodeManager::drawGraph() {
    return m_graphPtr->drawGraph();
}

ImVec4 NodeManager::getSlotColor(const std::string& vNodeSlotType) {
    ImVec4 res = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);

    if (m_ColorSlots.find(vNodeSlotType) != m_ColorSlots.end()) {
        res = m_ColorSlots.at(vNodeSlotType);
    }

    return res;
}

void NodeManager::addSlotColor(const std::string& vNodeSlotType, const ImVec4& vSlotColor) {
    m_ColorSlots[vNodeSlotType] = vSlotColor;
}