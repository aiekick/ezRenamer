#pragma once

#include <graph/base/baseStyle.h>
#include <graph/base/baseNode.h>

class ExecNode;
typedef std::shared_ptr<ExecNode> ExecNodePtr;
typedef std::weak_ptr<ExecNode> ExecNodeWeak;

class ExecNode : public BaseNode {
public:
    explicit ExecNode(const BaseStyle& vParentStyle, const BaseNodeDatas& vDatas);
    ENABLE_CLONE(ExecNode);
    bool init() override;
};
