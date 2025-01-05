#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>

#include <list>

class ExecSequence {
private:
    std::list<ExecNodeWeak> m_nodes;
    std::vector<BaseLinkWeak> m_links; // for display flow

public:
    void clear();
    void addNode(const ExecNodeWeak& vNode);
    void addLink(const BaseLinkWeak& vLink);
    bool executeFromFile(const std::string& vFile);
    bool executeFromText(const std::string& vText);

    // return true if the first node is an input
    // and if the last is an output
    // else return false
    bool ExecSequence::isValid() const;
};
