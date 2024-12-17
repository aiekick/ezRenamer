#pragma once

#include <ctools/cTools.h>
#include <imgui/imgui.h>

#include <string>
#include <unordered_map>
#include <map>

#include <future>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <list>

class BaseNode;
class NodeSlot;

struct ColumnContainerStruct
{
	std::map<int, std::weak_ptr<BaseNode>> nodes;
	ImVec2 size;
	ImVec2 offset;

	void AddNode(std::weak_ptr<BaseNode> vNode);
	void Clear();
};

class GraphLayout
{
public:
	static ImVec2 s_NodeSpacing;
	static float s_NodeCentering;

private:
	std::unordered_map<uint32_t, std::shared_ptr<BaseNode>> *m_NodesPtr = nullptr;
	std::map<int, ColumnContainerStruct> m_Columns;
	
public:
	void ApplyLayout(std::weak_ptr<BaseNode> vGraphNode);
	static bool DrawSettings();

private: 
	void CalcLayout(std::weak_ptr<BaseNode> vGraphNode);
	void ResetNodeStates();
	void ClassifyNodes(std::string vRootFunction);
	void SetColumnOfNodesRecurs(std::weak_ptr<BaseNode> vNode, ct::ivec2 vNodeCell);
	void AddNodesInCells();
	void AddNodeInCell(std::weak_ptr<BaseNode> vNode);
	void DefinePositionsOfNodes();

private: // security
	std::map<uintptr_t, int> m_InfLoopNodeDetector;
	bool IsThereAnInfiniteLoopForNode(std::weak_ptr<BaseNode> vNode); // recursive func SetColumnOfNodesRecurs

private: // final
	void ApplyPositionsInGraph();
	
public: // singleton
	static GraphLayout *Instance()
	{
		static GraphLayout _instance;
		return &_instance;
	}

protected:
	GraphLayout() = default; // Prevent construction
	GraphLayout(const GraphLayout&) {}; // Prevent construction by copying
	GraphLayout& operator =(const GraphLayout&) { return *this; }; // Prevent assignment
	~GraphLayout() = default; // Prevent unwanted destruction
};