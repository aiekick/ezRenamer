#include "nodeManager.h"
#include <graph/base/baseNode.h>
#include <graph/base/baseSlot.h>
#include <graph/library/nodesLibrary.h>

std::unique_ptr<NodeManager> NodeManager::mp_singleton = nullptr;

NodeManager* NodeManager::instance() {
    assert(mp_singleton != nullptr);
    return mp_singleton.get();
}

bool NodeManager::initInstance() {
    mp_singleton = std::make_unique<NodeManager>();
    return instance()->init();
}

void NodeManager::unitInstance() {
    instance()->unit();
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
    m_graphPtr->setBgRightClickAction(             //
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
                auto found_slot = new_node_ptr->findSlotByType(ez::SlotDir::INPUT, wanted_slot_type);
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
            }
        }
    }
}

