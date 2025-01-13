#include "nodeManager.h"
#include <ezlibs/ezImGui.hpp>
#include <project/projectFile.h>
#include <graph/base/baseNode.h>
#include <graph/base/baseSlot.h>
#include <graph/library/nodesLibrary.h>

std::unique_ptr<NodeManager> NodeManager::mp_singleton = nullptr;

NodeManager* NodeManager::Instance() {
    assert(mp_singleton != nullptr);
    return mp_singleton.get();
}

bool NodeManager::initInstance() {
    mp_singleton = std::make_unique<NodeManager>();
    return Instance()->init();
}

void NodeManager::unitInstance() {
    Instance()->unit();
    mp_singleton.reset();
}

bool NodeManager::init() {
    m_graphStyle.style.NodeRounding = 2.0f;
    m_graphStyle.style.NodeBorderWidth = 1.0f;
    m_graphStyle.style.altDragSnapping = 5.0f;
    m_graphConfig.showFlow = true;
    m_graphConfig.flowType = "FLOW";
    m_graphConfig.showFlowKey = ImGuiKey_Backspace;
    m_graphPtr = BaseGraph::create(m_graphStyle, m_graphConfig);
    m_graphPtr->setLoadNodeFromXmlFunctor(                                 //
        [this](const BaseGraphWeak& vGraph, const ez::xml::Node& vNode, const ez::xml::Node& vParent) {  //
            return m_loadNodeFromXml(vGraph, vNode, vParent);
        });
    m_graphPtr->setBgRightClickActionFunctor(      //
        [this](const BaseGraphWeak& /*vGraph*/) {  //
            m_showLibrary();
        });
    m_graphPtr->setPrepareForCreateNodeFromSlotActionFunctor(                         //
        [this](const BaseGraphWeak& /*vGraph*/, const BaseSlotWeak& vSlot) -> bool {  //
            m_createNodeFromSlot = vSlot;
            BaseLibrary::SlotType slot_type;
            if (!m_createNodeFromSlot.expired()) {
                slot_type = m_createNodeFromSlot.lock()->getDatas<BaseSlot::BaseSlotDatas>().type;
            }
            return m_filterLibraryForInputSlotType(slot_type);
        });

    addSlotColor("NONE", ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    addSlotColor("FLOW", ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    addSlotColor("FILE", ImVec4(0.5f, 0.5f, 0.9f, 1.0f));
    addSlotColor("STRING", ImVec4(0.9f, 0.9f, 0.1f, 1.0f));
    addSlotColor("TEXT", ImVec4(0.9f, 0.5f, 0.1f, 1.0f));

    m_nodesLibrary = NodesLibrary::get();

    return true;
}

void NodeManager::unit() {
    m_graphPtr.reset();
}

void NodeManager::clear() {
    m_graphPtr->clear();
}

bool NodeManager::drawGraph() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Settings")) {
            ImGui::MenuItem("Debug", nullptr, &m_graphStyle.debugMode);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Auto layout")) {
            m_baseLayout.applyLayout(m_graphPtr);
        }

        m_graphPtr->setCurrentEditor();

        if (nd::GetSelectedObjectCount()) {
            if (ImGui::BeginMenu("Selection")) {
                if (ImGui::MenuItem("Zoom on Selection")) {
                    m_graphPtr->zoomToSelection();
                }
                if (ImGui::MenuItem("Center on Selection")) {
                    m_graphPtr->navigateToSelection();
                }
                ImGui::EndMenu();
            }
        }

        if (ImGui::BeginMenu("Content")) {
            if (ImGui::MenuItem("Zoom on Content")) {
                m_graphPtr->zoomToContent();
            }

            if (ImGui::MenuItem("Center on Content")) {
                m_graphPtr->navigateToContent();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Style")) {
            m_baseLayout.drawSettings();
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (m_graphPtr->isGraphChanged()) {
        ProjectFile::Instance()->SetProjectChange();
        m_graphPtr->setGraphChanged(false);
    }

    return m_graphPtr->drawGraph();
}

BaseGraphWeak NodeManager::getGraph() const {
    return m_graphPtr;
}

bool NodeManager::getSlotColor(const std::string& vBaseSlotType, ImVec4& vOutColor) const {
    if (m_ColorSlots.find(vBaseSlotType) != m_ColorSlots.end()) {
        vOutColor = m_ColorSlots.at(vBaseSlotType);
        return true;
    }
    return false;
}

bool NodeManager::getSlotColor(const std::string& vBaseSlotType, ImU32& vOutColor) const {
    if (m_ColorSlots.find(vBaseSlotType) != m_ColorSlots.end()) {
        vOutColor = ImGui::GetColorU32(m_ColorSlots.at(vBaseSlotType));
        return true;
    }
    return false;
}

void NodeManager::addSlotColor(const std::string& vBaseSlotType, const ImVec4& vSlotColor) {
    m_ColorSlots[vBaseSlotType] = vSlotColor;
}

void NodeManager::drawDebugInfos() {
    m_graphPtr->drawDebugInfos();
}

ez::xml::Nodes NodeManager::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node xml;
    if (vUserDatas == "app") {
    } else if (vUserDatas == "project") {
        xml.addChilds(m_graphPtr->getXmlNodes());
    }
    return xml.getChildren();
}

bool NodeManager::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    const auto& strName = vNode.getName();
    const auto& strValue = vNode.getContent();
    const auto& strParentName = vParent.getName();
    if (vUserDatas == "app") {
    } else if (vUserDatas == "project") {
        m_graphPtr->setFromXmlNodes(vNode, vParent, vUserDatas);
    }
    return false;  // prevent xml node childs exploring
}

BaseNodeWeak NodeManager::createChildNodeInGraph(const BaseLibrary::NodeType& vNodeType, const BaseGraphWeak& vGraph) {
    return m_nodesLibrary.createChildNodeInGraph(vNodeType, vGraph);
}

void NodeManager::beforeXmlLoading() {
    m_graphPtr->beforeXmlLoading();
}

void NodeManager::afterXmlLoading() {
    m_graphPtr->afterXmlLoading();
}

bool NodeManager::m_loadNodeFromXml(const BaseGraphWeak& vGraph, const ez::xml::Node& vNode, const ez::xml::Node& vParent) {
    auto graph_ptr = vGraph.lock();
    if (graph_ptr != nullptr) {
        auto node_type = vNode.getAttribute("type");
        auto node_ptr = createChildNodeInGraph(node_type, vGraph).lock();
        if (node_ptr != nullptr) {
            node_ptr->beforeXmlLoading();
            node_ptr->setFromXmlNodes(vNode, vParent, {});
        }
    }
    return false;
}

bool NodeManager::m_filterLibraryForInputSlotType(const BaseLibrary::SlotType& vInputSlotType) {
    m_libraryToShow = m_nodesLibrary;
    if (!vInputSlotType.empty()) {
        return m_libraryToShow.filterNodesForSomeInputSlotTypes({vInputSlotType});
    }
    return false;
}

void NodeManager::m_showLibrary() {
    BaseLibrary::LibraryEntry entryToCreate;
    if (m_libraryToShow.showMenu(entryToCreate)) {
        BaseNodeWeak new_node = m_libraryToShow.createChildNodeInGraph(entryToCreate, m_graphPtr);
        // new node just created
        if (!new_node.expired()) {
            // if created node from slot mode
            // we will connect the slot to the first input slot 
            // of the corresponding type in the new node
            if (!m_createNodeFromSlot.expired()) {
                auto slot_ptr = m_createNodeFromSlot.lock();
                auto new_node_ptr = new_node.lock();
                auto wanted_slot_type = slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().type;
                auto wanted_slot_name = slot_ptr->getDatas<BaseSlot::BaseSlotDatas>().name;
                auto found_slot = new_node_ptr->findSlotByTypeAndOptionalName(ez::SlotDir::INPUT, wanted_slot_type, wanted_slot_name);
                // a slot of the good type was found
                // we will connect it
                if (!found_slot.expired()) {
                    m_graphPtr->connectSlots(m_createNodeFromSlot, found_slot);
                } else {
                    // we have filtered the list for this slot
                    // so if we not have it, its not normal
                    // and we must check what happen
                    LogVarDebugError("Fail to found a slot of type [%s] for node of type [%s]", wanted_slot_type.c_str(), entryToCreate.nodeType.c_str());
                }
                ProjectFile::Instance()->SetProjectChange();
            }
        }
    }
}

