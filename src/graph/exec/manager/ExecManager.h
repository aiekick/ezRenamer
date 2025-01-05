#pragma once

#include <graph/exec/sequence/ExecSequence.h>
#include <graph/base/baseGraph.h>
#include <graph/exec/nodes/OutputNode.h>
#include <graph/exec/nodes/InputNode.h>
#include <graph/exec/nodes/ToolNode.h>
#include <ezlibs/ezCnt.hpp>

class ExecManager {
private:
    std::vector<ExecSequence> m_sequences;

public:
    bool analyzeGraph(const BaseGraphWeak& vGraph);
    bool executeFromFile(const std::string& vFile);
    bool executeFromText(const std::string& vText);

private:
    typedef ez::cnt::DicoVector<ez::Uuid, InputNodeWeak> InputNodes;
    typedef ez::cnt::DicoVector<ez::Uuid, OutputNodeWeak> OutputNodes;
    typedef ez::cnt::DicoVector<ez::Uuid, ToolNodeWeak> ToolNodes;
    InputNodes m_getInputNodesFromGraph(const BaseGraphWeak& vGraph);
    OutputNodes m_getOutputNodesFromGraph(const BaseGraphWeak& vGraph);
    ToolNodes m_getToolNodesFromGraph(const BaseGraphWeak& vGraph);
};
