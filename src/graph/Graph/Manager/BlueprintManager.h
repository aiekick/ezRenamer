#pragma once

#include <Graph/Base/NodeSlot.h>
#include <ctools/cTools.h>

#include <set>
#include <string>

// cette class va permettre de gerer le systeme de generation d'ComputeRenderer
// a la fois son rendu et la gestion par nodegraph et la generation de code

class BaseNode;
class BlueprintManager
{
private:
	bool m_Loaded = false;

public: // init / unit
	bool Init();
	void Unit();

public: // render
	void Render();
	
public: // resize
	void Resize();

public: // imgui
	void DrawImGui();
	
public: // shaders
	void UpdateShaders(const std::set<std::string>& vFiles) const;
	void SetOrUpdateCode(std::string vCode);

	void SelectNodeForPreview(std::weak_ptr<BaseNode> vNode);

	// set camera / mouse
	void SetMousePos(ct::fvec2 vNormalizedPos, ct::fvec2 vBufferSize, bool vDownButton[5]);
	void SetScreenSize(ct::uvec2 vScreenSize);

public: // slots
	bool ConnectNodeSlots(std::weak_ptr<NodeSlot> vStart, std::weak_ptr<NodeSlot> vEnd);
	bool DisConnectNodeSlots(std::weak_ptr<NodeSlot> vStart, std::weak_ptr<NodeSlot> vEnd);

public: // singleton
	static BlueprintManager *Instance()
	{
		static BlueprintManager _instance;
		return &_instance;
	}

protected:
	BlueprintManager() {} // Prevent construction
	BlueprintManager(const BlueprintManager&) {}; // Prevent construction by copying
	BlueprintManager& operator =(const BlueprintManager&) { return *this; }; // Prevent assignment
	~BlueprintManager() {} // Prevent unwanted destruction
};