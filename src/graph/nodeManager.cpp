#include <graph/nodeManager.h>
#include <graph/base/baseNode.h>
#include <graph/base/baseSlot.h>

#include <graph/nodes/inputs/InputFileNode.h>
#include <graph/nodes/inputs/InputTextNode.h>
#include <graph/nodes/tools/SplitFilePath.h>
#include <graph/nodes/actions/FileNameRenamerNode.h>

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
    m_graphStyle.style.NodeRounding = 2.0f;
    m_graphStyle.style.NodeBorderWidth = 1.0f;
    m_graphStyle.style.altDragSnapping = 5.0f;

    addSlotColor("NONE", ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    addSlotColor("FLOW_SLOT", ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    addSlotColor("FILE_SLOT", ImVec4(0.5f, 0.5f, 0.9f, 1.0f));
    addSlotColor("STRING_SLOT", ImVec4(0.9f, 0.9f, 0.1f, 1.0f));

    m_baseLibrary.addLibraryEntry(BaseLibrary::LibraryEntry(
        "Sources",
        "Input files",
        "INPUT_FILE_NODE",  //
        {},
        {"FILE_SLOT"},
        BaseLibrary::NodeSource::INTERNAL));
    m_baseLibrary.addLibraryEntry(BaseLibrary::LibraryEntry(
        "Sources",
        "Input text",
        "INPUT_TEXT_NODE",  //
        {},
        {"STRING_SLOT"},
        BaseLibrary::NodeSource::INTERNAL));
    m_baseLibrary.addLibraryEntry(BaseLibrary::LibraryEntry(
        "Extractors",
        "File path splitter",
        "FILE_PATH_SPLITTER_NODE",  //
        {"FILE_SLOT"},
        {"FILE_SLOT", "STRING_SLOT"},
        BaseLibrary::NodeSource::INTERNAL));
    m_baseLibrary.addLibraryEntry(BaseLibrary::LibraryEntry(
        "Renamers",
        "File path name Renamer",
        "FILE_NAME_RENAMER_NODE",  //
        {"FILE_SLOT"},
        {},
        BaseLibrary::NodeSource::INTERNAL));

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

    return true;
}

void NodeManager::unit() {
    m_graphPtr.reset();
}

bool NodeManager::drawGraph() {
    return m_graphPtr->drawGraph();
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

bool NodeManager::executeGraph() {
    // on doit trouver les dernier nodes
    return false;
}

bool NodeManager::m_filterLibraryForInputSlotType(const BaseLibrary::SlotType& vInputSlotType) {
    m_libraryToShow = m_baseLibrary;
    if (!vInputSlotType.empty()) {
        return m_libraryToShow.filterNodesForSomeInputSlotTypes({vInputSlotType});
    }
    return false;
}

void NodeManager::m_showLibrary() {
    BaseLibrary::LibraryEntry entryToCreate;
    if (m_libraryToShow.showMenu(entryToCreate)) {
        BaseNodeWeak new_node;
        if (entryToCreate.nodeSource == BaseLibrary::NodeSource::INTERNAL) {
            new_node = m_createInternalNode(entryToCreate);
        } else if (entryToCreate.nodeSource == BaseLibrary::NodeSource::PLUGIN) {
            new_node = m_createPluginNode(entryToCreate);
        }
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

BaseNodeWeak NodeManager::m_createInternalNode(const BaseLibrary::LibraryEntry& vLibraryEntry) {
    if (vLibraryEntry.nodeType == "INPUT_FILE_NODE") {
        return m_graphPtr->createChildNode<InputFileNode>();
    } else if (vLibraryEntry.nodeType == "INPUT_TEXT_NODE") {
        return m_graphPtr->createChildNode<InputTextNode>();
    } else if (vLibraryEntry.nodeType == "FILE_PATH_SPLITTER_NODE") {
        return m_graphPtr->createChildNode<SplitFilePath>();
    } else if (vLibraryEntry.nodeType == "FILE_NAME_RENAMER_NODE") {
        return m_graphPtr->createChildNode<FileNameRenamerNode>();
    }
    return {};
}

BaseNodeWeak NodeManager::m_createPluginNode(const BaseLibrary::LibraryEntry& vLibraryEntry) {
    //nodePtr = PluginManager::Instance()->CreatePluginNode(vLibraryEntry.second.nodeType);
    return {};
}
