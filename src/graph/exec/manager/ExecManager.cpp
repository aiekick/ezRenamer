#include "ExecManager.h"
#include <graph/base/baseLink.h>
#include <graph/base/baseNode.h>
#include <graph/base/baseGraph.h>
#include <graph/exec/nodes/InputNode.h>
#include <graph/exec/nodes/OutputNode.h>
#include <graph/exec/nodes/SequenceNode.h>
#include <graph/exec/slots/FlowOutputSlot.h>

std::unique_ptr<ExecManager> ExecManager::mp_singleton = nullptr;

ExecManager* ExecManager::instance() {
    assert(mp_singleton != nullptr);
    return mp_singleton.get();
}

bool ExecManager::initInstance() {
    mp_singleton = std::make_unique<ExecManager>();
    return instance()->init();
}

void ExecManager::unitInstance() {
    instance()->unit();
    mp_singleton.reset();
}

bool ExecManager::init() {
    return true;
}

void ExecManager::unit() {}

// will analyse the graph and get all sequences 
// from inputNode to OutputNode
bool ExecManager::compileGraph(const BaseGraphWeak& vGraph) {
    m_sequences.clear();
    auto graph_ptr = vGraph.lock();
    if (graph_ptr != nullptr) {
        auto inputs = m_getInputNodesFromGraph(vGraph);
        // we will create a sequence for each inputs
        // explore by flow slot unitl we got a outputNode
        // then save the sequence
        for (const auto& input : inputs) {
            ExecSequence seq;
            m_followOutFlowSlotAndAddToSequenceRecurs(input, seq);
            if (seq.isValid()) {
                m_sequences.push_back(seq);
            }
        }
    }
    return (!m_sequences.empty());
}

// execute a sequence from a InputFile node
bool ExecManager::executeFromFile(const std::string& vFile) {
    return false;
}

// execute a sequence from a InputText node
bool ExecManager::executeFromText(const std::string& vText) {
    return false;
}

ExecManager::InputNodes ExecManager::m_getInputNodesFromGraph(const BaseGraphWeak& vGraph) {
    InputNodes ret;
    auto graph_ptr = vGraph.lock();
    if (graph_ptr != nullptr) {
        for (const auto& node : graph_ptr->getNodes()) {
            // check if its a InputNode
            auto node_ptr = std::dynamic_pointer_cast<InputNode>(node.lock());
            if (node_ptr != nullptr) {
                ret.tryAdd(node_ptr->getUuid(), node_ptr);
            }
        }
    }
    return ret;
}

ExecManager::OutputNodes ExecManager::m_getOutputNodesFromGraph(const BaseGraphWeak& vGraph) {
    OutputNodes ret;
    auto graph_ptr = vGraph.lock();
    if (graph_ptr != nullptr) {
        for (const auto& node : graph_ptr->getNodes()) {
            // check if its a OutputNode
            auto node_ptr = std::dynamic_pointer_cast<OutputNode>(node.lock());
            if (node_ptr != nullptr) {
                ret.tryAdd(node_ptr->getUuid(), node_ptr);
            }
        }
    }
    return ret;
}

ExecManager::ToolNodes ExecManager::m_getToolNodesFromGraph(const BaseGraphWeak& vGraph) {
    ToolNodes ret;
    auto graph_ptr = vGraph.lock();
    if (graph_ptr != nullptr) {
        for (const auto& node : graph_ptr->getNodes()) {
            // check if its a ToolNode
            auto node_ptr = std::dynamic_pointer_cast<ToolNode>(node.lock());
            if (node_ptr != nullptr) {
                ret.tryAdd(node_ptr->getUuid(), node_ptr);
            }
        }
    }
    return ret;
}

void ExecManager::m_followOutFlowSlotAndAddToSequenceRecurs(ExecNodeWeak vNode, ExecSequence& vOutSeq) {
    auto node_ptr = vNode.lock();
    if (node_ptr != nullptr) {
        vOutSeq.addNode(vNode);
        m_followOutFlowSlotAndAddToSequenceRecurs(node_ptr->getOutputFlowSlot(), vOutSeq);
        auto sequence_node_ptr = std::dynamic_pointer_cast<SequenceNode>(node_ptr);
        if (sequence_node_ptr != nullptr) {
            for (const auto& slot : sequence_node_ptr->m_getOutputSlots()) {
                auto flow_slot_ptr = std::dynamic_pointer_cast<FlowOutputSlot>(slot.lock());
                if (flow_slot_ptr != nullptr) {
                    m_followOutFlowSlotAndAddToSequenceRecurs(flow_slot_ptr, vOutSeq);
                }
            }
        }
    }
}

void ExecManager::m_followOutFlowSlotAndAddToSequenceRecurs(BaseSlotWeak vSlot, ExecSequence& vOutSeq) {
    auto out_flow_slot_ptr = vSlot.lock();
    if (out_flow_slot_ptr != nullptr && out_flow_slot_ptr->isConnected()) {
        for (const auto& link : out_flow_slot_ptr->getLinks()) {
            auto link_ptr = link.lock();
            if (link_ptr != nullptr) {
                auto in_flow_slot_ptr = link_ptr->getOutSlot().lock();
                if (in_flow_slot_ptr != nullptr) {
                    auto parent_node_ptr = in_flow_slot_ptr->getParentNode().lock();
                    if (parent_node_ptr != nullptr) {
                        m_followOutFlowSlotAndAddToSequenceRecurs(in_flow_slot_ptr->getParentNode<ExecNode>(), vOutSeq);
                    }
                }
            }
        }
    }
}
