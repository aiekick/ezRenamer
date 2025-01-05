#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>

#include <list>

class ExecSequence {
private:
    std::list<ExecNodeWeak> m_nodes;

public:
    void addNode(const ExecNodeWeak& vNode);
    bool executeFromFile(const std::string& vFile);
    bool executeFromText(const std::string& vText);
};
