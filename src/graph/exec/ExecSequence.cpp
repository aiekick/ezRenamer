#include "ExecSequence.h"
#include <graph/nodes/inputs/InputFileNode.h>
#include <graph/nodes/inputs/InputTextNode.h>

void ExecSequence::addNode(const ExecNodeWeak& vNode) {
    m_nodes.push_back(vNode);
}

bool ExecSequence::executeFromFile(const std::string& vFile) {
    if (!m_nodes.empty()) {
        // get first node
        auto node_ptr = m_nodes.front().lock();
        if (node_ptr != nullptr) {
            // check if its a InputFileNode
            auto input_file_node_ptr = std::dynamic_pointer_cast<InputFileNode>(node_ptr);
            if (input_file_node_ptr != nullptr) {
                EZ_TOOLS_DEBUG_BREAK;
            }
        }
    }
    return false;
}

bool ExecSequence::executeFromText(const std::string& vFile) {
    return false;
}
