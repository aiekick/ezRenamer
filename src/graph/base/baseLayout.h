#pragma once
#pragma warning(disable : 4251)

#include <ImGuiPack.h>
#include <graph/exec/nodes/abstracts/ExecNode.h>
#include <graph/base/baseDefs.h>
#include <graph/base/baseNode.h>
#include <ezlibs/ezCnt.hpp>
#include <cstdint>
#include <map>

class BaseLayout {
public:
    static ImVec2 s_NodeSpacing;
    static float s_NodeCentering;

private:
    struct ColumnContainer {
        std::map<int32_t, BaseNodeWeak> nodes;
        ImVec2 size;
        ImVec2 offset;
        void addNode(const BaseNodeWeak& vNode);
        void clear();
    };
    ez::cnt::DicoVector<ez::Uuid, BaseNodeWeak> m_nodes;
    std::map<int32_t, ColumnContainer> m_columns;
    std::map<ez::Uuid, int32_t> m_infLoopNodeDetector;

public:
    void applyLayout(const BaseGraphWeak& vGraphNode);
    void clear();
    bool drawSettings();

private:
    void m_calcLayout(const BaseGraphWeak& vGraph);
    void m_resetNodeStates();
    void m_classifyNodes(std::string vRootFunction);
    void m_setColumnOfNodesRecurs(const ExecNodeWeak& vNode, ez::ivec2 vNodeCell);
    void m_callInputSlot(const ez::SlotWeak& vSlot, const BaseNode::BaseNodeDatas& vNodeDatas, int32_t vCellIdx);
    void m_addNodesInCells();
    void m_addNodeInCell(const BaseNodeWeak& vNode);
    void m_definePositionsOfNodes();
    bool m_isThereAnInfiniteLoopForNode(const BaseNodeWeak& vNode);  // recursive func SetColumnOfNodesRecurs
    void m_applyPositionsInGraph(const BaseGraphWeak& vGraph);
};
