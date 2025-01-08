#include "baseLayout.h"
#include <ezlibs/ezLog.hpp>
#include <graph/base/baseSlot.h>
#include <graph/base/baseLink.h>
#include <graph/base/baseGraph.h>

/////////////////////////////////////////////////////////////////////////////////////
//// STATIC /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

ImVec2 BaseLayout::s_NodeSpacing = ImVec2(30, 30);
float BaseLayout::s_NodeCentering = 1.0f;

/////////////////////////////////////////////////////////////////////////////////////
//// PUBLIC /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void BaseLayout::applyLayout(const BaseGraphWeak &vGraph) {
    if (!vGraph.expired()) {
        auto graphPtr = vGraph.lock();
        if (graphPtr) {
            if (!graphPtr->getNodes().empty()) {
                m_nodes.clear();
                for (const auto &node : graphPtr->getNodes()) {
                    auto node_ptr = std::static_pointer_cast<BaseNode>(node.lock());
                    m_nodes.tryAdd(node_ptr->getUuid(), node_ptr);
                }
                m_calcLayout(vGraph);
                m_applyPositionsInGraph(vGraph);
                graphPtr->navigateToContent();
            }
        }
    }
    clear();
}

void BaseLayout::clear() {
    m_nodes.clear();
    m_columns.clear();
}

bool BaseLayout::drawSettings() {
    bool change = false;

    static ImVec2 defaultSpacing = BaseLayout::s_NodeSpacing;
    static float defaultCentering = BaseLayout::s_NodeCentering;

    if (ImGui::BeginMenu("Layout")) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Node Spacing");
        change |= ImGui::SliderFloatDefault(200, "Spacing X", &s_NodeSpacing.x, 1.0f, 1000.0f, defaultSpacing.x);
        change |= ImGui::SliderFloatDefault(200, "Spacing Y", &s_NodeSpacing.y, 1.0f, 1000.0f, defaultSpacing.y);
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Node Centering");
        change |= ImGui::SliderFloatDefault(200, "Centering X", &s_NodeCentering, 0.0f, 1.0f, defaultCentering);

        ImGui::EndMenu();
    }

    return change;
}

/////////////////////////////////////////////////////////////////////////////////////
//// BaseLayout::ColumnContainer ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void BaseLayout::ColumnContainer::addNode(const BaseNodeWeak &vNode) {
    if (!vNode.expired()) {
        auto node_ptr = vNode.lock();
        if (node_ptr != nullptr) {
            auto &node_datas = node_ptr->getDatasRef<BaseNode::BaseNodeDatas>();
            if (node_datas.layout.inserted) {
                LogVarDebugError("le node a deja été inséré.. (ColumnContainerStruct::AddNode)");
            }

            if (nodes.find(node_datas.layout.cell.y) != nodes.end()) {  // deja existant
                // on va trouver le celle.y max dans les ndoes
                int32_t mcy = node_datas.layout.cell.y;
                for (auto sec_node : nodes) {
                    if (!sec_node.second.expired()) {
                        auto secPtr = sec_node.second.lock();
                        if (secPtr) {
                            const auto &sec_node_datas = node_ptr->getDatas<BaseNode::BaseNodeDatas>();
                            mcy = ez::maxi(mcy, sec_node_datas.layout.cell.y);
                        }
                    }
                }
                // puis on set a mcy + 1
                node_datas.layout.cell.y = mcy + 1;
            }

            size.x = ez::maxi<float>(size.x, node_ptr->m_size.x);
            size.y += (nodes.empty() ? 0.0f : BaseLayout::s_NodeSpacing.y) + node_ptr->m_size.y;
            nodes[node_datas.layout.cell.y] = vNode;
            node_datas.layout.inserted = true;
        }
    }
}

void BaseLayout::ColumnContainer::clear() {
    nodes.clear();
    size = ImVec2(0, 0);
    offset = ImVec2(0, 0);
}

/////////////////////////////////////////////////////////////////////////////////////
//// PRIVATE ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void BaseLayout::m_calcLayout(const BaseGraphWeak &vGraph) {
    auto graph_ptr = vGraph.lock();
    if (graph_ptr) {
        m_infLoopNodeDetector.clear();
        m_columns.clear();
        m_resetNodeStates();
        m_classifyNodes("");
        m_addNodesInCells();
        m_definePositionsOfNodes();
    }
}

void BaseLayout::m_resetNodeStates() {
    if (!m_nodes.empty()) {
        // on doit effacer l'indicatif de colonne sinon rien ne sera mit en colonne
        // et le layout ne layoutera rien du tout du coup
        for (auto node : m_nodes) {
            auto node_ptr = node.lock();
            if (node_ptr != nullptr) {
                auto &node_datas = node_ptr->getDatasRef<BaseNode::BaseNodeDatas>();
                node_datas.layout.cell = ez::ivec2(-1);  // -1 pour que le 1er node (genre 'main') soit mit dans la colonne 0, sinon il serait ignoré
                node_datas.layout.inserted = false;      // pour voir si on insere le node plusieurs fois (utilité que pour debug)
                node_datas.layout.used = false;          // node utiisié ou non, pour cacher les nodes qui servent a rien
            }
        }
    }
}

void BaseLayout::m_classifyNodes(std::string vRootFunction) {
    if (!m_nodes.empty()) {
        // on parcours l'arbo on set les node a used is used, uniforms et calls
        // tres important, on isole les nodes qui servent a rien par used
        for (auto node : m_nodes) {
            auto node_ptr = std::static_pointer_cast<ExecNode>(node.lock());
            if (node_ptr != nullptr) {
                bool connected = false;
                for (auto &slot : node_ptr->m_getOutputSlotsRef()) {
                    auto slot_ptr = std::static_pointer_cast<BaseSlot>(slot.lock());
                    connected |= slot_ptr->isConnected();
                }
                auto out_flow_slot_ptr = node_ptr->getOutputFlowSlot().lock();
                if (out_flow_slot_ptr != nullptr) {
                    connected |= node_ptr->getOutputFlowSlot().lock()->isConnected();
                }
                if (!connected) {
                    auto &node_datas = node_ptr->getDatasRef<BaseNode::BaseNodeDatas>();

                    // le node racine est utilisé, sinon non et on le marque comme tel
                    // car apres on va propager cet etat a tout ces enfants
                    node_datas.layout.used = (node_datas.name == vRootFunction) || node_datas.layout.rootUsed;

                    // start node
                    m_setColumnOfNodesRecurs(node_ptr, 0);
                }
            }
        }
    }
}

void BaseLayout::m_callInputSlot(const ez::SlotWeak &vSlot, const BaseNode::BaseNodeDatas &vNodeDatas, int32_t vCellIdx) {
    auto slot_ptr = vSlot.lock();
    if (slot_ptr != nullptr) {
        if (slot_ptr->m_getConnectedSlots().size() == 1) {
            auto other_slot_ptr = slot_ptr->m_getConnectedSlots().begin()->lock();
            if (other_slot_ptr != nullptr) {
                auto other_node_ptr = std::static_pointer_cast<ExecNode>(other_slot_ptr->getParentNode().lock());
                if (other_node_ptr != nullptr) {
                    auto &other_node_datas = other_node_ptr->getDatasRef<BaseNode::BaseNodeDatas>();
                    if (vNodeDatas.layout.used) {  // on propage le used que si il est a true
                        other_node_datas.layout.used = vNodeDatas.layout.used;
                    }
                    m_setColumnOfNodesRecurs(other_node_ptr, vNodeDatas.layout.cell + ez::ivec2(1, vCellIdx));
                }
            }
        } else if (slot_ptr->m_getConnectedSlots().size() > 1) {
            LogVarDebugError("c'est pas normal qu'un call ait plusieurs inputs");
        }
    }
}

void BaseLayout::m_setColumnOfNodesRecurs(const ExecNodeWeak &vNode, ez::ivec2 vNodeCell) {
    if (vNode.expired() || m_isThereAnInfiniteLoopForNode(vNode)) {
        return;
    }
    auto node_ptr = vNode.lock();
    auto &node_datas = node_ptr->getDatasRef<BaseNode::BaseNodeDatas>();
    if (node_datas.layout.cell.x < vNodeCell.x) {
        node_datas.layout.cell = vNodeCell;

        // on dit ou doit etre placé le node
        // position qui pourra etre réevaluée plus tard, si un enfant l'utilse aussi
        // dans ce cas il ira dans la colonne d'apres l'enfant
        // c'est pour ca qu'on ne les mets pas tout de suite dans leur cellule
        // sinon ca pourrai cree un ajout du meme node dans plusieurs colonnes
        // compliquant de facto la regle : un node ne doit appartenir qu'a une seule colonne
    }

    // call childs
    int32_t cellIdx = 0;
    for (const auto &slot : node_ptr->m_getInputSlots()) {
        m_callInputSlot(slot, node_datas, cellIdx);
        ++cellIdx;
    }
    m_callInputSlot(node_ptr->getInputFlowSlot(), node_datas, cellIdx);
    ++cellIdx;
}

void BaseLayout::m_addNodesInCells() {
    if (!m_nodes.empty()) {
        for (auto node : m_nodes) {
            m_addNodeInCell(node);
        }
    }
}

void BaseLayout::m_addNodeInCell(const BaseNodeWeak &vNode) {
    auto node_ptr = vNode.lock();
    if (node_ptr) {
        auto &node_datas = node_ptr->getDatasRef<BaseNode::BaseNodeDatas>();
        m_columns[node_datas.layout.cell.x].addNode(vNode);
    }
}

void BaseLayout::m_definePositionsOfNodes() {
    ImVec2 lastOffset;

    for (auto &columnPair : m_columns) {
        auto &column = columnPair.second;

        // on defini l'offset de la colonne
        if (columnPair.first == 0) {
            column.offset.x = 0.0f;
        } else {
            column.offset.x = lastOffset.x - s_NodeSpacing.x - column.size.x;
        }
        column.offset.y = column.size.y * -0.5f;  // centrage des colonne en y

        // on va centrer les nodes en x dans la colonne
        // po va placer les nodes en y les uns a la suite des autres
        int nodeIdx = 0;
        float nextNodeBottomPosY = column.offset.y;
        for (auto &nodePair : column.nodes) {
            const auto &node = nodePair.second;
            if (!node.expired()) {
                auto nodePtr = node.lock();
                if (nodePtr) {
                    nextNodeBottomPosY += ((nodeIdx == 0) ? 0.0f : s_NodeSpacing.y);  // on ajoute l'espace ci besoin au curseur
                    nodePtr->m_pos.x = column.offset.x + column.size.x * s_NodeCentering - nodePtr->m_size.x * s_NodeCentering;  // centrage x du node dans la colonne
                    nodePtr->m_pos.y = nextNodeBottomPosY;                                                                      // position du node en y

                    nodeIdx++;
                    nextNodeBottomPosY += nodePtr->m_size.y;  // on place le curseur de position au bas du node que l'on vient de placer
                }
            }
        }

        lastOffset = column.offset;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// SECURITY ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

bool BaseLayout::m_isThereAnInfiniteLoopForNode(const BaseNodeWeak &vNode) {  // recursive func SetColumnOfNodesRecurs
    bool res = false;
    auto node_ptr = vNode.lock();
    if (node_ptr != nullptr) {
        auto node_id = node_ptr->getUuid();
        if (m_infLoopNodeDetector.find(node_id) == m_infLoopNodeDetector.end()) {
            m_infLoopNodeDetector[node_id] = 0;
        } else {
            ++m_infLoopNodeDetector[node_id];
        }
        if (m_infLoopNodeDetector[node_id] > 50) {
            const auto &node_datas = node_ptr->getDatas<BaseNode::BaseNodeDatas>();
            LogVarDebugError("Maybe, we have an infinite loop for node %s", node_datas.name.c_str());
            res = true;
        }
    }
    return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// FINAL ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void BaseLayout::m_applyPositionsInGraph(const BaseGraphWeak &vGraph) {
    if (!m_nodes.empty()) {
        vGraph.lock()->setCurrentEditor();
        for (auto node : m_nodes) {
            auto node_ptr = std::static_pointer_cast<BaseNode>(node.lock());
            if (node_ptr != nullptr) {
                nd::SetNodePosition(node_ptr->getUuid(), node_ptr->m_pos);
            }
        }
    }
}
