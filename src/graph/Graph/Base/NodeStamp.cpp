// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "NodeStamp.h"
#include <imgui/imgui.h>

NodeStamp::NodeStamp()
{
	
}

NodeStamp::~NodeStamp()
{
	
}

void NodeStamp::DrawImGui()
{
	ImGui::Text("type stamp : %s", typeStamp.c_str()); // float(vec3)
	ImGui::Text("name stamp : %s", nameStamp.c_str()); // float map(vec3)
	ImGui::Text("full stamp : %s", fullStamp.c_str()); // float map(vec3 a)
}