#include "ExecNode.h"

ExecNode::ExecNode(const BaseStyle& vParentStyle, const BaseNodeDatas& vDatas)  //
    : BaseNode(vParentStyle, vDatas) {}

bool ExecNode::init() {
    return BaseNode::init();
}
