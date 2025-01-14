#include "GeneratorNode.h"

GeneratorNode::GeneratorNode(const BaseStyle& vParentStyle)  //
    : Parent(vParentStyle) {}

bool GeneratorNode::init() {
    bool ret = Parent::init();
    getDatasRef<BaseNodeDatas>().color = ImGui::GetColorU32(ImVec4(0.8f, 0.2f, 0.8f, 1.0f));
    return ret;
}

