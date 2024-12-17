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
    /* m_graphPtr = BaseNode::create({});
    auto node = m_graphPtr->createChildNode<BaseNode>({});
    auto ptr = node.lock();
    if (ptr != nullptr) {
        ptr->addSlot<BaseSlot>(BaseSlot::SlotConfig("in0", "base", ez::SlotDir::INPUT));
        ptr->addSlot<BaseSlot>(BaseSlot::SlotConfig("out0", "base", ez::SlotDir::OUTPUT));
    }*/
    return true;
}

void NodeManager::unit() {
    //m_graphPtr.reset();
}

bool NodeManager::drawGraph() {
    return true;
    //return m_graphPtr->drawGraph();
}

void NodeManager::m_displayBlueprintNodesMenu() {
}
