// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GraphLayout.h"
#include <Graph/Base/BaseNode.h>
#include <Gui/ImWidgets.h>

/////////////////////////////////////////////////////////////////////////////////////
//// STATIC /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

ImVec2 GraphLayout::s_NodeSpacing = ImVec2(50, 20);
float GraphLayout::s_NodeCentering = 1.0f;

/////////////////////////////////////////////////////////////////////////////////////
//// PUBLIC /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void GraphLayout::ApplyLayout(std::weak_ptr<BaseNode> vGraphNode)
{
	if (!vGraphNode.expired())
	{
		auto graphPtr = vGraphNode.lock();
		if (graphPtr)
		{
			if (!graphPtr->m_ChildNodes.empty())
			{
				m_NodesPtr = &graphPtr->m_ChildNodes; // on prend le temps de l'execution de cette fonction

				CalcLayout(vGraphNode);

				ApplyPositionsInGraph();
				graphPtr->NavigateToContent();

				m_NodesPtr = nullptr;
			}
		}
	}
}

bool GraphLayout::DrawSettings()
{
	bool change = false;

	static ImVec2 defaultSpacing = GraphLayout::s_NodeSpacing;
	static float defaultCentering = GraphLayout::s_NodeCentering;

	if (ImGui::BeginMenu("Layout"))
	{
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
//// ColumnContainerStruct //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void ColumnContainerStruct::AddNode(std::weak_ptr<BaseNode> vNode)
{
	if (!vNode.expired())
	{
		auto ptr = vNode.lock();
		if (ptr)
		{
			if (ptr->inserted)
			{
				LogVarDebug("le node a deja �t� ins�r�.. (ColumnContainerStruct::AddNode)");
			}

			if (nodes.find(ptr->cell.y) != nodes.end()) // deja existant
			{
				// on va trouver le celle.y max dans les ndoes
				int mcy = ptr->cell.y;
				for (auto node : nodes)
				{
					if (!node.second.expired())
					{
						auto secPtr = node.second.lock();
						if (secPtr)
						{
							mcy = ct::maxi(mcy, secPtr->cell.y);
						}
					}
				}
				// puis on set a mcy + 1
				ptr->cell.y = mcy + 1;
			}

			size.x = ct::maxi<float>(size.x, ptr->size.x);
			size.y += (nodes.empty() ? 0.0f : GraphLayout::s_NodeSpacing.y) + ptr->size.y;
			nodes[ptr->cell.y] = vNode;
			ptr->inserted = true;
		}
	}
}

void ColumnContainerStruct::Clear()
{
	nodes.clear();
	size = ImVec2(0, 0);
	offset = ImVec2(0, 0);
}

/////////////////////////////////////////////////////////////////////////////////////
//// PRIVATE ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void GraphLayout::CalcLayout(std::weak_ptr<BaseNode> vGraphNode)
{
	if (!vGraphNode.expired())
	{
		auto ptr = vGraphNode.lock();
		if (ptr)
		{
			m_InfLoopNodeDetector.clear();
			m_Columns.clear();
			ResetNodeStates();
			ClassifyNodes(ptr->rootFuncName);
			AddNodesInCells();
			DefinePositionsOfNodes();
		}
	}
}

void GraphLayout::ResetNodeStates()
{
	if (m_NodesPtr)
	{
		// on doit effacer l'indicatif de colonne sinon rien ne sera mit en colonne
		// et le layout ne layoutera rien du tout du coup
		for (auto nodePair : *m_NodesPtr)
		{
			auto node = nodePair.second;
			if (node)
			{
				node->cell = ct::ivec2(-1); // -1 pour que le 1er node (genre 'main') soit mit dans la colonne 0, sinon il serait ignor�
				node->inserted = false;  // pour voir si on insere le node plusieurs fois (utilit� que pour debug)
				node->used = false; // node utiisi� ou non, pour cacher les nodes qui servent a rien
			}
		}
	}
}

void GraphLayout::ClassifyNodes(std::string vRootFunction)
{
	if (m_NodesPtr)
	{
		// on parcours l'arbo on set les node a used is used, uniforms et calls
		// tres important, on isole les nodes qui servent a rien par used
		for (auto nodePair : *m_NodesPtr)
		{
			auto node = nodePair.second;
			if (node)
			{
				bool connected = false;
				for (auto &slot : node->m_Slots)
				{
					connected |= slot.second->connected;
				}
				if (!connected)
				{
					// le node racine est utilis�, sinon non et on le marque comme tel
					// car apres on va propager cet etat a tout ces enfants
					node->used = (node->name == vRootFunction) || node->rootUsed;

					// start node
					SetColumnOfNodesRecurs(node, 0);
				}
			}
		}
	}
}

void GraphLayout::SetColumnOfNodesRecurs(std::weak_ptr<BaseNode> vNode, ct::ivec2 vNodeCell)
{
	if (vNode.expired()) return;
	if (IsThereAnInfiniteLoopForNode(vNode)) return;
	auto nodePtr = vNode.lock();
	if (!nodePtr) return;

	if (nodePtr->cell.x < vNodeCell.x)
	{
		nodePtr->cell = vNodeCell;

		// on dit ou doit etre plac� le node
		// position qui pourra etre r�evalu�e plus tard, si un enfant l'utilse aussi
		// dans ce cas il ira dans la colonne d'apres l'enfant
		// c'est pour ca qu'on ne les mets pas tout de suite dans leur cellule
		// sinon ca pourrai cree un ajout du meme node dans plusieurs colonnes
		// compliquant de facto la regle : un node ne doit appartenir qu'a une seule colonne
	}

	int cellIdx = 0;

	// flow childs
	for (const auto &flow : nodePtr->m_Slots)
	{
		if (flow.second->links.size() == 1)
		{
			auto otherSlot = *flow.second->links.begin();
			if (!otherSlot.expired())
			{
				auto otherSlotPtr = otherSlot.lock();
				if (otherSlotPtr)
				{
					if (!otherSlotPtr->parentNode.expired())
					{
						auto parentPtr = otherSlotPtr->parentNode.lock();
						if (parentPtr)
						{
							if (nodePtr->used) // on propage le used que si il est a true
								parentPtr->used = nodePtr->used;
							SetColumnOfNodesRecurs(otherSlotPtr->parentNode, nodePtr->cell + ct::ivec2(1, cellIdx));
						}
					}
				}
			}
		}
		else if (flow.second->links.size() > 1)
		{
			LogVarDebug("c'est pas normal qu'un flow ait plusieurs inputs");
		}
		cellIdx++;
	}
}

void GraphLayout::AddNodesInCells()
{
	if (m_NodesPtr)
	{
		for (auto & nodePair : *m_NodesPtr)
		{
			auto node = nodePair.second;
			if (node)
			{
				AddNodeInCell(node);
			}
		}
	}
}

void GraphLayout::AddNodeInCell(std::weak_ptr<BaseNode> vNode)
{
	if (!vNode.expired())
	{
		auto nodePtr = vNode.lock();
		if (nodePtr)
		{
			m_Columns[nodePtr->cell.x].AddNode(vNode);
		}
	}
}

void GraphLayout::DefinePositionsOfNodes()
{
	ImVec2 lastOffset;

	for (auto & columnPair : m_Columns)
	{
		auto & column = columnPair.second;

		// on defini l'offset de la colonne
		if (columnPair.first == 0)
		{
			column.offset.x = 0.0f;
		}
		else
		{
			column.offset.x = lastOffset.x - s_NodeSpacing.x - column.size.x;
		}
		column.offset.y = column.size.y * -0.5f; // centrage des colonne en y

		// on va centrer les nodes en x dans la colonne
		// po va placer les nodes en y les uns a la suite des autres
		int nodeIdx = 0;
		float nextNodeBottomPosY = column.offset.y;
		for (auto & nodePair : column.nodes)
		{
			const auto & node = nodePair.second;
			if (!node.expired())
			{
				auto nodePtr = node.lock();
				if (nodePtr)
				{
					nextNodeBottomPosY += ((nodeIdx == 0) ? 0.0f : s_NodeSpacing.y); // on ajoute l'espace ci besoin au curseur
					nodePtr->pos.x = column.offset.x + column.size.x * s_NodeCentering - nodePtr->size.x * s_NodeCentering; // centrage x du node dans la colonne
					nodePtr->pos.y = nextNodeBottomPosY; // position du node en y 

					nodeIdx++;
					nextNodeBottomPosY += nodePtr->size.y; // on place le curseur de position au bas du node que l'on vient de placer
				}
			}
		}

		lastOffset = column.offset;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// SECURITY ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

bool GraphLayout::IsThereAnInfiniteLoopForNode(std::weak_ptr<BaseNode> vNode) // recursive func SetColumnOfNodesRecurs
{
	bool res = false;

	if (!vNode.expired())
	{
		auto nodePtr = vNode.lock();
		if (nodePtr)
		{
			uintptr_t nodeId = nodePtr->nodeID;
			if (m_InfLoopNodeDetector.find(nodeId) == m_InfLoopNodeDetector.end())
			{
				m_InfLoopNodeDetector[nodeId] = 0;
			}
			else
			{
				m_InfLoopNodeDetector[nodeId]++;
			}

			if (m_InfLoopNodeDetector[nodeId] > 50)
			{
				LogVarDebug("Maybe, we have an infinite loop for node %s", nodePtr->name.c_str());
				res = true;
			}
		}
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// FINAL ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void GraphLayout::ApplyPositionsInGraph()
{
	if (m_NodesPtr)
	{
		namespace nd = ax::NodeEditor;
		for (const auto &node : *m_NodesPtr)
		{
			//SetNodePosition(node.second->nodeID, node.second->pos);
		}
	}
}
