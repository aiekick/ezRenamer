#pragma once

#include <graph/exec/ExecSequence.h>
#include <graph/base/baseGraph.h>

class ExecManager {
private:
    std::vector<ExecSequence> m_sequences;

public:
    bool analyzeGraph(const BaseGraphWeak& vGraph);
    bool executeFromFile(const std::string& vFile);
    bool executeFromText(const std::string& vText);
};
