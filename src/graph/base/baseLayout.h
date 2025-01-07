#pragma once
#pragma warning(disable : 4251)

#include <ImGuiPack.h>
#include <graph/base/baseDefs.h>
#include <ezlibs/ezCnt.hpp>
#include <map>
#include <cstdint>

class BaseLayout {
public:
    static ImVec2 s_NodeSpacing;
    static float s_NodeCentering;

private:
    struct ColumnContainer {
        std::map<int, BaseNodeWeak> nodes;
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
    void m_setColumnOfNodesRecurs(const BaseNodeWeak& vNode, ez::ivec2 vNodeCell);
    void m_addNodesInCells();
    void m_addNodeInCell(const BaseNodeWeak& vNode);
    void m_definePositionsOfNodes();
    bool m_isThereAnInfiniteLoopForNode(const BaseNodeWeak& vNode);  // recursive func SetColumnOfNodesRecurs
    void m_applyPositionsInGraph();
};
