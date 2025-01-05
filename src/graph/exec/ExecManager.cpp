#include "ExecManager.h"
#include <graph/exec/nodes/InputNode.h>
#include <graph/exec/nodes/ActionNode.h>

// will analyse the graph and get all sequences 
// from inputNode to ActionNode
bool ExecManager::analyzeGraph(const BaseGraphWeak& vGraph) {
    auto graph_ptr = vGraph.lock();
    if (graph_ptr != nullptr) {
        for (const auto& node : graph_ptr->getNodes()) {
            // check if its a InputNode
            auto input_node_ptr = std::dynamic_pointer_cast<InputNode>(node.lock());
            if (input_node_ptr != nullptr) {
         
            }
        }
    }
    return false;
}

// execute a sequence from a InputFile node
bool ExecManager::executeFromFile(const std::string& vFile) {
    return false;
}

// execute a sequence from a InputText node
bool ExecManager::executeFromText(const std::string& vText) {
    return false;
}
