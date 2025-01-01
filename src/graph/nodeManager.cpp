#include <graph/nodeManager.h>
#include <graph/base/baseNode.h>
#include <graph/base/baseSlot.h>

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
    m_graphStyle.style.NodePadding = 10.0f;
    m_graphStyle.style.NodeRounding = 2.0f;
    addSlotColor("NONE", ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    addSlotColor("FILES", ImVec4(0.5f, 0.5f, 0.9f, 1.0f));
    addSlotColor("TOKEN", ImVec4(0.9f, 0.9f, 0.1f, 1.0f));
    m_graphPtr = BaseGraph::create(m_graphStyle, m_graphConfig);
    auto node = m_graphPtr->createChildNode<BaseNode>(m_graphStyle, {});
    auto ptr = node.lock();
    if (ptr != nullptr) {
        ptr->addSlot<BaseSlot>(m_graphStyle, BaseSlot::BaseSlotDatas("in0", "base", ez::SlotDir::INPUT));
        ptr->addSlot<BaseSlot>(m_graphStyle, BaseSlot::BaseSlotDatas("out0", "base", ez::SlotDir::OUTPUT));
    }
    return true;
}

void NodeManager::unit() {
    m_graphPtr.reset();
}

bool NodeManager::drawGraph() {
    return m_graphPtr->drawGraph();
}

ImVec4 NodeManager::getSlotColor(const std::string& vBaseSlotType) {
    ImVec4 res = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);

    if (m_ColorSlots.find(vBaseSlotType) != m_ColorSlots.end()) {
        res = m_ColorSlots.at(vBaseSlotType);
    }

    return res;
}

void NodeManager::addSlotColor(const std::string& vBaseSlotType, const ImVec4& vSlotColor) {
    m_ColorSlots[vBaseSlotType] = vSlotColor;
}