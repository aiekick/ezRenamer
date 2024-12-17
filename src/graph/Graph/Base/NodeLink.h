#pragma once

#include <Graph/uTypes/uTypes.h>
#include <imgui/imgui.h>
#include <string>
#include <set>
#include <memory>

#include <imgui_node_editor/NodeEditor/Include/imgui_node_editor.h>
namespace nd = ax::NodeEditor;

class BaseNode;
class NodeSlot;
class NodeLink
{
public:
	std::weak_ptr<NodeSlot> in;
	std::weak_ptr<NodeSlot> out;
	uint32_t linkId = 0;
	ImColor color = ImColor(255, 255, 0, 255);
	float thick = 2.0f;

public:
	NodeLink();
	~NodeLink();
};