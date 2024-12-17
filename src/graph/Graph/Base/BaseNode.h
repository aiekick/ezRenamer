#pragma once

#include <ctools/ConfigAbstract.h>
#include "NodeSlot.h"
#include "NodeLink.h"
#include <Graph/uTypes/uTypes.h>
#include <ctools/cTools.h>
#include <imgui/imgui.h>
#include <ctools/Logger.h>

#include <imgui_node_editor/NodeEditor/Source/imgui_canvas.h>

#include <imgui/imgui_internal.h>

#include <unordered_map>
#include <map>
#include <utility>
#include <memory>
#include <string>
#include <set>
#include <unordered_set>
#include <array>
#include <functional>

#define MAGIC_NUMBER 4577

/*
POUR EFFACER LES NODES
il faut appeler la fonction DestroyNodesIfAnys() apres le rendu de imgui voir meme apres le swap buffer ou swapchain
*/

enum StyleColor
{
	StyleColor_Bg,
	StyleColor_Grid,
	StyleColor_NodeBg,
	StyleColor_NodeBorder,
	StyleColor_HovNodeBorder,
	StyleColor_SelNodeBorder,
	StyleColor_NodeSelRect,
	StyleColor_NodeSelRectBorder,
	StyleColor_HovLinkBorder,
	StyleColor_SelLinkBorder,
	StyleColor_LinkSelRect,
	StyleColor_LinkSelRectBorder,
	StyleColor_PinRect,
	StyleColor_PinRectBorder,
	StyleColor_Flow,
	StyleColor_FlowMarker,
	StyleColor_GroupBg,
	StyleColor_GroupBorder,

	StyleColor_Count
};

enum StyleVar
{
	StyleVar_NodePadding,
	StyleVar_NodeRounding,
	StyleVar_NodeBorderWidth,
	StyleVar_HoveredNodeBorderWidth,
	StyleVar_SelectedNodeBorderWidth,
	StyleVar_PinRounding,
	StyleVar_PinBorderWidth,
	StyleVar_LinkStrength,
	StyleVar_SourceDirection,
	StyleVar_TargetDirection,
	StyleVar_ScrollDuration,
	StyleVar_FlowMarkerDistance,
	StyleVar_FlowSpeed,
	StyleVar_FlowDuration,
	StyleVar_PivotAlignment,
	StyleVar_PivotSize,
	StyleVar_PivotScale,
	StyleVar_PinCorners,
	StyleVar_PinRadius,
	StyleVar_PinArrowSize,
	StyleVar_PinArrowWidth,
	StyleVar_GroupRounding,
	StyleVar_GroupBorderWidth,
};

struct GraphStyleStruct
{
	ImVec4 NodePadding;
	float NodeRounding;
	float NodeBorderWidth;
	float HoveredNodeBorderWidth;
	float SelectedNodeBorderWidth;
	float PinRounding;
	float PinBorderWidth;
	float LinkStrength;
	ImVec2 SourceDirection;
	ImVec2 TargetDirection;
	float ScrollDuration;
	float FlowMarkerDistance;
	float FlowSpeed;
	float FlowDuration;
	ImVec2 PivotAlignment;
	ImVec2 PivotSize;
	ImVec2 PivotScale;
	float PinCorners;
	float PinRadius;
	float PinArrowSize;
	float PinArrowWidth;
	float GroupRounding;
	float GroupBorderWidth;
	ImVec4 Colors[StyleColor_Count];
	float ZoomSpeed;

	GraphStyleStruct()
	{
		NodePadding = ImVec4(8, 8, 8, 8);
		NodeRounding = 4.0f;
		NodeBorderWidth = 1.5f;
		HoveredNodeBorderWidth = 3.5f;
		SelectedNodeBorderWidth = 3.5f;
		PinRounding = 4.0f;
		PinBorderWidth = 0.0f;
		LinkStrength = 100.0f;
		SourceDirection = ImVec2(1.0f, 0.0f);
		TargetDirection = ImVec2(-1.0f, 0.0f);
		ScrollDuration = 0.35f;
		FlowMarkerDistance = 30.0f;
		FlowSpeed = 150.0f;
		FlowDuration = 2.0f;
		PivotAlignment = ImVec2(0.5f, 0.5f);
		PivotSize = ImVec2(0.0f, 0.0f);
		PivotScale = ImVec2(1, 1);
		PinCorners = ImDrawFlags_RoundCornersAll;
		PinRadius = 0.0f;
		PinArrowSize = 0.0f;
		PinArrowWidth = 0.0f;
		GroupRounding = 6.0f;
		GroupBorderWidth = 1.0f;
		ZoomSpeed = 0.1f;

		Colors[StyleColor_Bg] = ImColor(60, 60, 70, 200);
		Colors[StyleColor_Grid] = ImColor(120, 120, 120, 40);
		Colors[StyleColor_NodeBg] = ImColor(32, 32, 32, 200);
		Colors[StyleColor_NodeBorder] = ImColor(255, 255, 255, 96);
		Colors[StyleColor_HovNodeBorder] = ImColor(50, 176, 255, 255);
		Colors[StyleColor_SelNodeBorder] = ImColor(255, 176, 50, 255);
		Colors[StyleColor_NodeSelRect] = ImColor(5, 130, 255, 64);
		Colors[StyleColor_NodeSelRectBorder] = ImColor(5, 130, 255, 128);
		Colors[StyleColor_HovLinkBorder] = ImColor(50, 176, 255, 255);
		Colors[StyleColor_SelLinkBorder] = ImColor(255, 176, 50, 255);
		Colors[StyleColor_LinkSelRect] = ImColor(5, 130, 255, 64);
		Colors[StyleColor_LinkSelRectBorder] = ImColor(5, 130, 255, 128);
		Colors[StyleColor_PinRect] = ImColor(60, 180, 255, 100);
		Colors[StyleColor_PinRectBorder] = ImColor(60, 180, 255, 128);
		Colors[StyleColor_Flow] = ImColor(255, 128, 64, 255);
		Colors[StyleColor_FlowMarker] = ImColor(255, 128, 64, 255);
		Colors[StyleColor_GroupBg] = ImColor(0, 0, 0, 160);
		Colors[StyleColor_GroupBorder] = ImColor(255, 255, 255, 32);
	}
};

struct OldFuncToChangeStruct
{
	std::string parentFunc;
	std::string callName;
	std::string stamp;
	std::string sourceFunc;
	std::string sourceFullFunc;
	ct::uvec2 sourceLoc;
	size_t start = 0;
	size_t end = 0;

	OldFuncToChangeStruct()
	{
		start = 0;
		end = 0;
	}
};

struct Func_Loc_In_Code_Struct
{
	std::string parentFunc;
	std::string stamp;
	std::string sourceFunc;
	std::vector<ct::uvec2> locFunc;
	ct::uvec2 sourceLoc;
	std::string sourceFullFunc;
	std::vector<ct::uvec2> locFullFunc;
	//size_t start;
	//size_t end;
	std::string replacingFuncName;
	std::string replacingFullFunc;

	Func_Loc_In_Code_Struct() = default;
};

enum class LINK_TYPE_Enum : uint8_t
{
	LINK_TYPE_LINE = 0,
	LINK_TYPE_SPLINE,
	LINK_TYPE_STRAIGHT,
	LINK_TYPE_Count
};

enum class NodeTypeEnum : uint8_t
{
	NODE_TYPE_NONE = 0,
	NODE_TYPE_FUNCTION, // un node de fonction (qui peut contenir d'autre graph)
	NODE_TYPE_Count,
};
inline static std::string GetStringFromNodeTypeEnum(const NodeTypeEnum& vNodeTypeEnum)
{
	static std::array<std::string, (uint8_t)NodeTypeEnum::NODE_TYPE_Count> NodeTypeString = {
		"NONE",
		"FUNCTION", // un node de fonction (qui peut contenir d'autre graph)
	};
	if (vNodeTypeEnum != NodeTypeEnum::NODE_TYPE_Count)
		return NodeTypeString[(int)vNodeTypeEnum];
	LogVarDebug("Error, one NoteTypeEnum have no corresponding string, return \"None\"");
	return "NONE";
}

inline static NodeTypeEnum GetNodeTypeEnumFromString(const std::string& vNodeTypeString)
{
	if (vNodeTypeString == "NONE") return NodeTypeEnum::NODE_TYPE_NONE;
	else if (vNodeTypeString == "FUNCTION") return NodeTypeEnum::NODE_TYPE_FUNCTION;
	return NodeTypeEnum::NODE_TYPE_NONE;
}

struct BaseNodeStateStruct
{
	GraphStyleStruct graphStyle;

	bool debug_mode = false;

	ImVec2 scrolling;

	int itemPushId = -1; // imgui widget id

	bool hoveredItem = false;
	bool activeItem = false;

	bool is_any_hovered_items = false;
	bool is_any_active_items = false;

	std::weak_ptr<BaseNode> current_hovered_node;
	std::weak_ptr<BaseNode> current_moving_node;
	std::weak_ptr<BaseNode> current_selected_node;

	LINK_TYPE_Enum linkType = LINK_TYPE_Enum::LINK_TYPE_SPLINE;
	bool showLinks = true;
	bool showLinksBehind = true;
	bool showUnUsedNodes = true;
	bool showUniforms = true;
	bool showExtractedFuncs = true;
	bool showFunctionParametersSlots = true;
	bool showFunctionSlots = true;

	std::weak_ptr<NodeSlot> linkFromSlot; // quand on prend un lien depuis un slot
	
	std::weak_ptr<BaseNode> node_to_open;
	std::weak_ptr<BaseNode> node_to_select;

	// custom context menu
	bool m_CustomContextMenuRequested = false;
	std::weak_ptr<BaseNode> m_CustomContextMenuNode;
};

class GenericRenderer;
class BaseNode : public conf::ConfigAbstract
{
public:
	static std::shared_ptr<BaseNode> Create();

public:
	std::weak_ptr<BaseNode> m_This;

public: // links
	std::unordered_map<uint32_t, std::shared_ptr<NodeLink>> m_Links; // linkId, link // for search query
	std::unordered_map<uint32_t, std::set<uint32_t>> m_LinksDico; // NodeSlot Ptr, linkId // for search query

public: // static
	static uint32_t freeNodeId;
	static uint32_t GetNextNodeId();

	static std::shared_ptr<BaseNode> GetSharedFromWeak(std::weak_ptr<BaseNode> vNode);

	static std::function<void(std::weak_ptr<BaseNode>)> sOpenGraphCallback; // open graph
	static void BaseNode::OpenGraph_Callback(std::weak_ptr<BaseNode> vNode);

	static std::function<void(std::string)> sOpenCodeCallback; // open code
	static void BaseNode::OpenCode_Callback(std::string vCode);

	static std::function<void(std::string)> sLogErrorsCallback; // log errors
	static void BaseNode::LogErrors_Callback(std::string vErrors);

	static std::function<void(std::string)> sLogInfosCallback; // log infos
	static void BaseNode::LogInfos_Callback(std::string vInfos);

public: // popups
	bool m_CreateNewNode = false;
	ImVec2 m_OpenPopupPosition;
	uint32_t m_ContextMenuNodeId = 0;
	uint32_t m_ContextMenuSlotId = 0;
	uint32_t m_ContextMenuLinkId = 0;
	ImRect m_HeaderRect = ImRect(0,0,0,0);

public: // ident 
	// on met ca en prems pour le voir direct dans le debugger quand on est au dessu d'un abstractNode
	// on verra le nomen 1er ou second
	std::string name; // nom raccouris du node, genre nom de la focntion
	uint32_t nodeID;
	uType::uTypeEnum returnType = uType::uTypeEnum::U_VOID; // type du retour
	std::string uniquePaneId;
	
public:
	BaseNodeStateStruct m_BaseNodeState;
	NodeTypeEnum m_NodeType = NodeTypeEnum::NODE_TYPE_NONE;

public: // used by layout
	ImVec2 pos; // position absolue du node
	ImVec2 size; // taille du node
	bool used = false; // utilis� dans le code ou non
	bool hidden = false; // visibilit� du node
	ct::ivec2 cell = ct::ivec2(-1); // layout x:column, y:row
	bool inserted = false; // pour voir si il y a des doublon dasn des colonnes
	std::string rootFuncName = "main"; // le return du graph, genre "main" ou un return
	bool rootUsed = false; // ce node est le root
	bool graphDisabled = false; // pas possible d'ouvirr ce graph
	bool deletionDisabled = false; // pas possible d'ffacer ce node
	bool changed = false; // need to save

public: // parsing de la fonction pas du grpah complet
	std::string funcNameToParse = "";
	bool mainFuncParsing = false;

public: // Code Generation
	ct::uvec2 m_CodeBlock; // x:start / y:end in m_Code

public: // glslang and links
	int m_Depth = 0; // glslang
	std::weak_ptr<BaseNode> m_ParentNode; // node parent dans le cas d'un ndoe enfant d'un graph
	std::unordered_map<uint32_t, std::shared_ptr<BaseNode>> m_ChildNodes; // node du graphn  // for query only
	//std::string m_lastChildNodeName; // le nom du dernier node pour le layout
	
	std::map<uint32_t, std::shared_ptr<NodeSlot>> m_Slots; // for display, need order

	std::set<uint32_t> m_NodeIdToDelete; // node selected to delete

	ImVec2 m_BaseCopyOffset = ImVec2(0, 0);
	std::vector<uint32_t> m_NodesToCopy; // for copy/paste

private:
	bool m_IsCodeDirty = false; // code changed, need regeneration of code of parents
	ImGuiEx::Canvas m_Canvas;
	int m_ExternalChannel = 0;
	uint32_t m_DoubleClickedNode = 0;
	uint32_t m_DoubleClickedPin = 0;
	uint32_t m_DoubleClickedLink = 0;
	bool m_BackgroundClicked = false;
	bool m_BackgroundDoubleClicked = false;

	bool m_RequestContextMenu_Background = false;
	uint32_t m_RequestContextMenu_PinId = 0;
	uint32_t m_RequestContextMenu_NodeId = 0;
	uint32_t m_RequestContextMenu_LinkId = 0;
	bool m_IsDragging = false;
	ImVec2 m_DrawStartPoint;


public:
	std::unordered_map<std::string, uint32_t> m_NodeStamps; // bd des signatures (de fonctions, params, return, etc..) for query only

public:
	std::string m_NodeGraphConfigFile;

public:
	BaseNode();
	virtual ~BaseNode();

public:
    virtual bool Init();
	virtual bool Init(std::weak_ptr<BaseNode> vThis);
	virtual bool Init(std::string vCode, std::weak_ptr<BaseNode> vThis);
	void InitGraph();
	void FinalizeGraphLoading();

	virtual void Unit();
	void UnitGraph();

	void ClearNode();
	void ClearGraph();
	void ClearSlots();

	virtual void UpdateSlots();
	virtual void ClearDescriptors();

	bool DrawGraph(); // dras graph of child
	bool GenerateGraphFromCode(const std::string& vCode);

	void ZoomToContent() const;
	void NavigateToContent() const;
	void ZoomToSelection() const;
	void NavigateToSelection() const;

	ImVec2 GetCanvasOffset() const;

	void CopySelectedNodes();
	void PasteNodesAtMousePos();
	void DuplicateSelectedNodes(ImVec2 vOffset = ImVec2(0,0));

	void Suspend();
	void Resume();

	// finders
	std::weak_ptr<BaseNode> FindNode(uint32_t vId);
	std::weak_ptr<BaseNode> FindNodeByName(std::string vName);
	std::vector<std::weak_ptr<BaseNode>> GetPublicNodes();		// les Public nodes sont les nodes exposé dans le parents, c'est PUBLIC_ puis non du node
	std::weak_ptr<NodeLink> FindLink(uint32_t vId);
	std::weak_ptr<NodeSlot> FindSlot(uint32_t vId);
	std::weak_ptr<NodeSlot> FindNodeSlotByName(std::weak_ptr<BaseNode> vNode, std::string vName);
	
	// Add slots
	std::weak_ptr<NodeSlot> AddSlot(NodeSlot vSlot, bool vIncSlotId = false, bool vHideName = true);

	// add nodes
	std::weak_ptr<BaseNode> AddChildNode(std::shared_ptr<BaseNode> vNode, bool vIncNodeId = false);

	// // get the root node by exploring tree with by slots
	std::shared_ptr<BaseNode> GetRootNodeBySlots(std::shared_ptr<BaseNode> vNode = 0);

	// node removal
	void DestroyChildNode(std::weak_ptr<BaseNode> vNode);
	bool DestroyChildNodeByIdIfAllowed(int vNodeID, bool vDestroy);
	void DestroySlotOfAnyMap(std::weak_ptr<NodeSlot> vSlot);

	// delete node finally
	void DestroyNodesIfAnys(); // a executer apres le rendu de imgui

	// need to save
	void SetChanged(bool vFlag = true);

public:
	void DoLayout();

public: // shader gen related stuff
	virtual std::string GetNodeCode(bool vRecursChilds = true);
	virtual void CompilGeneratedCode();
	virtual void JustConnectedBySlots(std::weak_ptr<NodeSlot> vStartSlot, std::weak_ptr<NodeSlot> vEndSlot);
	virtual void JustDisConnectedBySlots(std::weak_ptr<NodeSlot> vStartSlot, std::weak_ptr<NodeSlot> vEndSlot);
	bool IsCodeDirty();
	void SetCodeDirty(bool vFlag);
	
private: // graph states / action / drawings
	void DoGraphActions(BaseNodeStateStruct *vCanvasState);
	void OpenNodeInNewPane(BaseNodeStateStruct* vCanvasState);
	void SelectNodeforPreview(BaseNodeStateStruct* vCanvasState);
	void FillState(BaseNodeStateStruct *vCanvasState);
	void DoCreateLinkOrNode(BaseNodeStateStruct *vCanvasState);
	void DoDeleteLinkOrNode(BaseNodeStateStruct *vCanvasState);
	void DoShorcutsOnNode(BaseNodeStateStruct *vCanvasState);
	bool ShowNodeContextMenu(uint32_t *vNodeId);
	bool ShowPinContextMenu(uint32_t *vPinId);
	bool ShowLinkContextMenu(uint32_t *vLinkId);
	bool ShowBackgroundContextMenu();
	void DoPopups(BaseNodeStateStruct *vCanvasState);
	void DoCheckNodePopup(BaseNodeStateStruct *vCanvasState);
	void DoCheckSlotPopup(BaseNodeStateStruct *vCanvasState);
	void DoCheckLinkPopup(BaseNodeStateStruct *vCanvasState);
	void DoNewNodePopup(BaseNodeStateStruct *vCanvasState);

private: // graph
	void ResetActions();
	void ManageContextMenus();
	void ManageZoom();
	void ManageDragging();
	void ManageActions();
	void DrawGridAndBackground();

private: // node manipulation
	void DuplicateNode(uint32_t vNodeId, ImVec2 vOffsetPos);

private: // utils
	std::string GetAvailableNodeStamp(const std::string& vNodeStamp);

public: // Get Links / Slots
	std::vector<std::weak_ptr<NodeLink>> GetLinksAssociatedToSlot(std::weak_ptr<NodeSlot> vSlot);
	std::vector<std::weak_ptr<NodeSlot>> GetSlotsAssociatedToSlot(std::weak_ptr<NodeSlot> vSlot);

public: // ADD/DELETE VISUAL LINKS (NO CHANGE BEHIND)
	void Add_VisualLink(std::weak_ptr<NodeSlot> vStart, std::weak_ptr<NodeSlot> vEnd);
	void Del_VisualLink(uint32_t vLinkId);
	void Break_VisualLinks_ConnectedToSlot(std::weak_ptr<NodeSlot> vSlot);
	void Break_VisualLink_ConnectedToSlots(std::weak_ptr<NodeSlot> vFrom, std::weak_ptr<NodeSlot> vTo);
	
public: // CONNECT / DISCONNECT SLOTS BEHIND
	bool ConnectSlots(std::weak_ptr<NodeSlot> vFrom, std::weak_ptr<NodeSlot> vTo);
	bool DisConnectSlots(std::weak_ptr<NodeSlot> vFrom, std::weak_ptr<NodeSlot> vTo);
	bool DisConnectSlot(std::weak_ptr<NodeSlot> vSlot);
	virtual void NotifyConnectionChangeOfThisSlot(std::weak_ptr<NodeSlot> vSlot, bool vConnected); // ce solt a été connecté/déconnecté

public: // test if slot connection possible for have rule node
	virtual bool CanWeConnectSlots(std::weak_ptr<NodeSlot> vFrom, std::weak_ptr<NodeSlot> vTo);

public: // slot splitter
	virtual std::vector<std::weak_ptr<NodeSlot>> InjectTypeInSlot(std::weak_ptr<NodeSlot> vSlotToSplit, uType::uTypeEnum vType);

public: // ImGui
	void DrawStyleMenu(); // ImGui
	void DrawNodeGraphStyleMenu() const; // imgui_node_editor
	void DrawToolMenu();

public: // pane
	virtual bool DrawDebugInfos(BaseNodeStateStruct *vCanvasState);
	virtual void DrawProperties(BaseNodeStateStruct* vCanvasState);

public: // draw nodes widgets
	virtual void DrawInputWidget(BaseNodeStateStruct *vCanvasState, std::weak_ptr<NodeSlot> vSlot);
	virtual void DrawOutputWidget(BaseNodeStateStruct *vCanvasState, std::weak_ptr<NodeSlot> vSlot);
	virtual void DrawContextMenuForSlot(BaseNodeStateStruct *vCanvasState, std::weak_ptr<NodeSlot> vSlot);
	virtual void DrawContextMenuForNode(BaseNodeStateStruct *vCanvasState);
	virtual void DrawCustomContextMenuForNode(BaseNodeStateStruct *vCanvasState);
	
protected: // draw graph links
	virtual void DisplayInfosOnTopOfTheNode(BaseNodeStateStruct *vCanvasState);
	virtual void DrawNode(BaseNodeStateStruct *vCanvasState);
	virtual bool DrawBegin(BaseNodeStateStruct *vCanvasState);
	virtual bool DrawHeader(BaseNodeStateStruct *vCanvasState);
	virtual bool DrawNodeContent(BaseNodeStateStruct *vCanvasState);
	virtual bool DrawFooter(BaseNodeStateStruct *vCanvasState);
	virtual bool DrawEnd(BaseNodeStateStruct *vCanvasState);
	virtual void DrawLinks(BaseNodeStateStruct *vCanvasState);
	
public: // loading / saving
	typedef std::pair<uint32_t, std::string> SlotEntry;
	typedef std::pair<SlotEntry, SlotEntry> LinkEntry;
	std::vector<LinkEntry> m_LinksToBuildAfterLoading;
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;
	bool LoadNode(
		tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas,
		std::string vName, NodeTypeEnum vNodeType, ct::fvec2 vPos, size_t vNodeId);
};