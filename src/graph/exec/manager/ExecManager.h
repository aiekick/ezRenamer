#pragma once

#include <vector>
#include <memory>

#include <grapher/grapher.h>
#include <graph/exec/nodes/ToolNode.h>
#include <graph/exec/nodes/InputNode.h>
#include <graph/exec/nodes/OutputNode.h>
#include <graph/exec/sequence/ExecSequence.h>
#include <graph/exec/nodes/abstracts/ExecNode.h>

class ExecManager {
private:  // Static
    static std::unique_ptr<ExecManager> mp_singleton;

public:  // Static
    static ExecManager* Instance();
    static bool initInstance();
    static void unitInstance();

private:
    std::vector<ExecSequence> m_sequences;

public:
    bool init();
    void unit();

    bool compileGraph(const BaseGraphWeak& vGraph);
    bool executeFromFile(const std::string& vFile);
    bool executeFromText(const std::string& vText);

private:
    typedef ez::cnt::DicoVector<ez::Uuid, InputNodeWeak> InputNodes;
    typedef ez::cnt::DicoVector<ez::Uuid, OutputNodeWeak> OutputNodes;
    typedef ez::cnt::DicoVector<ez::Uuid, ToolNodeWeak> ToolNodes;
    InputNodes m_getInputNodesFromGraph(const BaseGraphWeak& vGraph);
    OutputNodes m_getOutputNodesFromGraph(const BaseGraphWeak& vGraph);
    ToolNodes m_getToolNodesFromGraph(const BaseGraphWeak& vGraph);
    void m_followOutFlowSlotAndAddToSequenceRecurs(ExecNodeWeak vNode, ExecSequence& vOutSeq);
    void m_followOutFlowSlotAndAddToSequenceRecurs(BaseSlotWeak vSlot, ExecSequence& vOutSeq);
};
