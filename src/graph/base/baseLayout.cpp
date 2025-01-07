#include "baseLayout.h"
#include <graph/base/baseNode.h>
#include <graph/base/baseLink.h>
#include <graph/base/baseGraph.h>

/////////////////////////////////////////////////////////////////////////////////////
//// STATIC /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

ImVec2 BaseLayout::s_NodeSpacing = ImVec2(70, 30);
float BaseLayout::s_NodeCentering = 1.0f;

/////////////////////////////////////////////////////////////////////////////////////
//// PUBLIC /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void BaseLayout::applyLayout(const BaseGraphWeak& vGraph) {
    if (!vGraph.expired()) {
        auto graphPtr = vGraph.lock();
        if (graphPtr) {
            if (!graphPtr->getNodes().empty()) {
                m_nodes = &graphPtr->getNodes();  // on prend le temps de l'execution de cette fonction

                m_calcLayout(vGraph);

                    ApplyPositionsInGraph();
                    graphPtr->NavigateToContent();

                m_nodes = nullptr;
            }
        }
    }

    clear();
}

void BaseLayout::clear() {
    m_nodes = nullptr;
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
        if (node_ptr!=nullptr) {
            const auto &node_datas = node_ptr->getDatas<BaseNode::BaseNodeDatas>();
            if (node_datas.layout.inserted) {
                LogVarDebugError("le node a deja été inséré.. (ColumnContainerStruct::AddNode)");
            }

            if (nodes.find(node_datas.layout.cell.y) != nodes.end()) 
            {  // deja existant
                // on va trouver le celle.y max dans les ndoes
                int mcy = node_datas.layout.cell.y;
                for (auto sec_node : nodes) {
                    if (!sec_node.second.expired()) {
                        auto secPtr = sec_node.second.lock();
                        if (secPtr) {
                            const auto &sec_node_datas = node_ptr->getDatas<BaseNode::BaseNodeDatas>();
                            mcy = ez::maxi(mcy, secPtr->cell.y);
                        }
                    }
                }
                // puis on set a mcy + 1
                node_datas.layout.cell.y = mcy + 1;
            }

            size.x = ez::maxi<float>(size.x, node_ptr->m_size.x);
            size.y += (nodes.empty() ? 0.0f : BaseLayout::s_NodeSpacing.y) + ptr->m_size.y;
            nodes[ptr->cell.y] = vNode;
            ptr->inserted = true;
        }
    }
}

void BaseLayout::ColumnContainer::clear() {
    nodes.clear();
    m_size = ImVec2(0, 0);
    offset = ImVec2(0, 0);
}

/////////////////////////////////////////////////////////////////////////////////////
//// PRIVATE ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void BaseLayout::m_calcLayout(const BaseGraphWeak &vGraph) {
    if (!vGraph.expired()) {
        auto ptr = vGraph.lock();
        if (ptr) {
            m_infLoopNodeDetector.clear();
            m_columns.clear();
            m_resetNodeStates();
            m_classifyNodes(ptr->rootFuncName);
            m_addNodesInCells();
            m_definePositionsOfNodes();
        }
    }
}

void BaseLayout::m_resetNodeStates() {
    if (m_nodes) {
        // on doit effacer l'indicatif de colonne sinon rien ne sera mit en colonne
        // et le layout ne layoutera rien du tout du coup
        for (auto nodePair : *m_nodes) {
            auto node = nodePair.second;
            if (node) {
                node->cell = ez::ivec2(-1);  // -1 pour que le 1er node (genre 'main') soit mit dans la colonne 0, sinon il serait ignoré
                node->inserted = false;      // pour voir si on insere le node plusieurs fois (utilité que pour debug)
                node->used = false;          // node utiisié ou non, pour cacher les nodes qui servent a rien
            }
        }
    }
}

void BaseLayout::m_classifyNodes(std::string vRootFunction) {
    if (m_nodes) {
        // on parcours l'arbo on set les node a used is used, uniforms et calls
        // tres important, on isole les nodes qui servent a rien par used
        for (auto nodePair : *m_nodes) {
            auto node = nodePair.second;
            if (node) {
                bool connected = false;
                for (auto &slot : node->m_Outputs) {
                    connected |= slot.second->connected;
                }
                if (!connected) {
                    // le node racine est utilisé, sinon non et on le marque comme tel
                    // car apres on va propager cet etat a tout ces enfants
                    node->used = (node->name == vRootFunction) || node->rootUsed;

                    // start node
                    SetColumnOfNodesRecurs(node, 0);
                }
            }
        }
    }
}

void BaseLayout::m_setColumnOfNodesRecurs(const BaseNodeWeak &vNode, ez::ivec2 vNodeCell) {
    if (vNode.expired())
        return;
    if (IsThereAnInfiniteLoopForNode(vNode))
        return;
    auto nodePtr = vNode.lock();
    if (!nodePtr)
        return;

    if (nodePtr->cell.x < vNodeCell.x) {
        nodePtr->cell = vNodeCell;

        // on dit ou doit etre placé le node
        // position qui pourra etre réevaluée plus tard, si un enfant l'utilse aussi
        // dans ce cas il ira dans la colonne d'apres l'enfant
        // c'est pour ca qu'on ne les mets pas tout de suite dans leur cellule
        // sinon ca pourrai cree un ajout du meme node dans plusieurs colonnes
        // compliquant de facto la regle : un node ne doit appartenir qu'a une seule colonne
    }

    int cellIdx = 0;

    // call childs
    for (const auto &call : nodePtr->m_Inputs) {
        if (call.second->linkedSlots.m_size() == 1) {
            auto otherSlot = *call.second->linkedSlots.begin();
            if (!otherSlot.expired()) {
                auto otherSlotPtr = otherSlot.lock();
                if (otherSlotPtr) {
                    if (!otherSlotPtr->parentNode.expired()) {
                        auto parentPtr = otherSlotPtr->parentNode.lock();
                        if (parentPtr) {
                            if (nodePtr->used)  // on propage le used que si il est a true
                                parentPtr->used = nodePtr->used;
                            SetColumnOfNodesRecurs(otherSlotPtr->parentNode, nodePtr->cell + ez::ivec2(1, cellIdx));
                        }
                    }
                }
            }
        } else if (call.second->linkedSlots.m_size() > 1) {
            LogVarDebugError("c'est pas normal qu'un call ait plusieurs inputs");
        }
        cellIdx++;
    }
}

void BaseLayout::m_addNodesInCells() {
    if (m_nodes) {
        for (auto &nodePair : *m_nodes) {
            auto node = nodePair.second;
            if (node) {
                m_addNodeInCell(node);
            }
        }
    }
}

void BaseLayout::m_addNodeInCell(const BaseNodeWeak &vNode) {
    if (!vNode.expired()) {
        auto nodePtr = vNode.lock();
        if (nodePtr) {
            m_columns[nodePtr->cell.x].AddNode(vNode);
        }
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
            column.offset.x = lastOffset.x - s_NodeSpacing.x - column.m_size.x;
        }
        column.offset.y = column.m_size.y * -0.5f;  // centrage des colonne en y

        // on va centrer les nodes en x dans la colonne
        // po va placer les nodes en y les uns a la suite des autres
        int nodeIdx = 0;
        float nextNodeBottomPosY = column.offset.y;
        for (auto &nodePair : column.nodes) {
            const auto &node = nodePair.second;
            if (!node.expired()) {
                auto nodePtr = node.lock();
                if (nodePtr) {
                    nextNodeBottomPosY += ((nodeIdx == 0) ? 0.0f : s_NodeSpacing.y);                                         // on ajoute l'espace ci besoin au curseur
                    nodePtr->m_pos.x = column.offset.x + column.m_size.x * s_NodeCentering - nodePtr->m_size.x * s_NodeCentering;  // centrage x du node dans la colonne
                    nodePtr->m_pos.y = nextNodeBottomPosY;                                                                     // position du node en y

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

bool BaseLayout::m_isThereAnInfiniteLoopForNode(const BaseNodeWeak &vNode)  // recursive func SetColumnOfNodesRecurs
{
    bool res = false;

    if (!vNode.expired()) {
        auto nodePtr = vNode.lock();
        if (nodePtr) {
            uintptr_t nodeId = nodePtr->GetNodeID();
            if (m_InfLoopNodeDetector.find(nodeId) == m_InfLoopNodeDetector.end()) {
                m_InfLoopNodeDetector[nodeId] = 0;
            } else {
                m_InfLoopNodeDetector[nodeId]++;
            }

            if (m_InfLoopNodeDetector[nodeId] > 50) {
                LogVarDebugError("Maybe, we have an infinite loop for node %s", nodePtr->name.c_str());
                res = true;
            }
        }
    }

    return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// FINAL ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void BaseLayout::m_applyPositionsInGraph() {
    if (m_nodes) {
        namespace nd = ax::NodeEditor;
        for (const auto &node : *m_nodes) {
            auto nodePtr = node.second;
            if (nodePtr) {
                nd::SetNodePosition(nodePtr->m_nodeID, nodePtr->m_pos);
            }
        }
    }
}
