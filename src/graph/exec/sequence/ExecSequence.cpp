#include "ExecSequence.h"
#include <graph/nodes/inputs/InputFileNode.h>
#include <graph/nodes/inputs/InputTextNode.h>
#include <graph/exec/nodes/InputNode.h>
#include <graph/exec/nodes/OutputNode.h>

void ExecSequence::clear() {
    m_nodes.clear();
    m_links.clear();
}

void ExecSequence::addNode(const ExecNodeWeak& vNode) {
    m_nodes.push_back(vNode);
}

void ExecSequence::addLink(const BaseLinkWeak& vLink) {
    m_links.push_back(vLink);
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
    if (!m_nodes.empty()) {
        // get first node
        auto node_ptr = m_nodes.front().lock();
        if (node_ptr != nullptr) {
            // check if its a InputFileNode
            auto input_file_node_ptr = std::dynamic_pointer_cast<InputTextNode>(node_ptr);
            if (input_file_node_ptr != nullptr) {
                EZ_TOOLS_DEBUG_BREAK;
            }
        }
    }
    return false;
}

bool ExecSequence::isValid() const {
    bool ret = false;
    auto first_input_node_ptr = std::dynamic_pointer_cast<InputNode>(m_nodes.front().lock());
    auto last_output_node_ptr = std::dynamic_pointer_cast<OutputNode>(m_nodes.back().lock());
    return (first_input_node_ptr != nullptr) && (last_output_node_ptr != nullptr);
}

