#pragma once

#include <graph/exec/nodes/abstracts/ExecNode.h>

class GeneratorNode : public ExecNode {
    typedef ExecNode Parent;

public:
    explicit GeneratorNode(const BaseStyle& vParentStyle);
    ENABLE_CLONE(GeneratorNode);
    bool init() override;
};

typedef std::shared_ptr<GeneratorNode> GeneratorNodePtr;
typedef std::weak_ptr<GeneratorNode> GeneratorNodeWeak;
