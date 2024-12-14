#include <graph/nodeManager.h>
#include <filesystem>
namespace fs = std::filesystem;

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
    return true;
}

void NodeManager::unit() {
}

bool NodeManager::drawGraph() {
    m_canvas.begin();
    m_canvas.drawGrid(m_gridConfig);
    m_canvas.end();
    return false;
}