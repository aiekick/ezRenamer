#pragma once

#include <string>

class NodeStamp
{
public:
	std::string typeStamp; // float(vec3)
	std::string nameStamp; // float map(vec3)
	std::string fullStamp; // float map(vec3 a)
	
public:
	NodeStamp();
	~NodeStamp();

	void DrawImGui();
};