// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "BaseNode.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <Gui/ImWidgets.h>
#include <Graph/Library/UserNodeLibrary.h>
#include <Graph/Manager/BlueprintManager.h>

static bool showNodeStyleEditor = false;
uint32_t BaseNode::freeNodeId = MAGIC_NUMBER;

//////////////////////////////////////////////////////////////////////////////
////// STATIC ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

uint32_t BaseNode::GetNextNodeId()
{
	return ++BaseNode::freeNodeId;
}

std::shared_ptr<BaseNode> BaseNode::GetSharedFromWeak(std::weak_ptr<BaseNode> vNode)
{
	std::shared_ptr<BaseNode> res = 0;

	if (!vNode.expired())
	{
		res = vNode.lock();
	}

	return res;
}

std::function<void(std::weak_ptr<BaseNode>)> BaseNode::sOpenGraphCallback;
void BaseNode::OpenGraph_Callback(std::weak_ptr<BaseNode> vNode)
{
	if (BaseNode::sOpenGraphCallback)
	{
		BaseNode::sOpenGraphCallback(vNode);
	}
}

std::function<void(std::string)> BaseNode::sOpenCodeCallback;
void BaseNode::OpenCode_Callback(std::string vCode)
{
	if (BaseNode::sOpenCodeCallback)
	{
		BaseNode::sOpenCodeCallback(vCode);
	}
}

std::function<void(std::string)> BaseNode::sLogErrorsCallback;
void BaseNode::LogErrors_Callback(std::string vErrors)
{
	if (BaseNode::sLogErrorsCallback)
	{
		BaseNode::sLogErrorsCallback(vErrors);
	}
}

std::function<void(std::string)> BaseNode::sLogInfosCallback;
void BaseNode::LogInfos_Callback(std::string vInfos)
{
	if (BaseNode::sLogInfosCallback)
	{
		BaseNode::sLogInfosCallback(vInfos);
	}
}
//////////////////////////////////////////////////////////////////////////////
////// CREATE ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::shared_ptr<BaseNode> BaseNode::Create()
{
	auto res = std::make_shared<BaseNode>();
	res->m_This = res;

	res->name = "baseNode";
	res->m_Depth = 1;

	//res->Unit();
	//res.reset();

	return res;
}
//////////////////////////////////////////////////////////////////////////////
////// CONSTRUCTOR ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

BaseNode::BaseNode()
{
	m_NodeType = NodeTypeEnum::NODE_TYPE_NONE;
	nodeID = GetNextNodeId();
}

BaseNode::~BaseNode()
{
	Unit();
}

//////////////////////////////////////////////////////////////////////////////
////// INIT //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool BaseNode::Init()
{
	InitGraph();

	return true;
}

bool BaseNode::Init(std::weak_ptr<BaseNode> vThis)
{
	m_This = vThis;

	return true;
}

bool BaseNode::Init(std::string vCode, std::weak_ptr<BaseNode> vThis)
{
	UNUSED(vCode);

	m_This = vThis;

	InitGraph();

	return true;
}

void BaseNode::InitGraph()
{

} 

//////////////////////////////////////////////////////////////////////////////
////// DESTROY NODE / GRAPH //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::Unit()
{
	ClearNode();
	UnitGraph();
}

void BaseNode::UnitGraph()
{
	ClearGraph();
	m_LinksToBuildAfterLoading.clear();
}

// on va cree les link avec le chargement du xml
void BaseNode::FinalizeGraphLoading()
{
	for (auto entry : m_LinksToBuildAfterLoading)
	{
		SlotEntry entIn = entry.first;
		SlotEntry entOut = entry.second;

		auto inNode = FindNode(entIn.first);
		auto outNode = FindNode(entOut.first);
		
		if (!inNode.expired() && !outNode.expired())
		{
			auto inSlot = FindNodeSlotByName(inNode, entIn.second);
			auto outSlot = FindNodeSlotByName(outNode, entOut.second);

			ConnectSlots(inSlot, outSlot);
		}
	}
}

void BaseNode::DoGraphActions(BaseNodeStateStruct *vCanvasState)
{
	if (m_BackgroundClicked)
	{
		vCanvasState->node_to_select.reset();
		vCanvasState->current_selected_node.reset();
	}

	OpenNodeInNewPane(vCanvasState);
	SelectNodeforPreview(vCanvasState);
}

void BaseNode::OpenNodeInNewPane(BaseNodeStateStruct* vCanvasState)
{
	// open node in new pane
	if (!vCanvasState->node_to_open.expired())
	{
		auto nodePtr = vCanvasState->node_to_open.lock();
		if (nodePtr)
		{
			// dans notre cas, si le graph est vide c'est pas grave, car on doit pouvoir y ajouter du code
			//if (!vCanvasState->node_to_open->m_ChildNodes.empty())
			if (!nodePtr->graphDisabled)
			{
				OpenGraph_Callback(vCanvasState->node_to_open);
			}

			vCanvasState->node_to_open.reset();
		}
	}
}

void BaseNode::SelectNodeforPreview(BaseNodeStateStruct* vCanvasState)
{
	// select node for preview
	if (!vCanvasState->node_to_select.expired())
	{
		auto current_selected_nodePtr = vCanvasState->current_selected_node.lock();
		auto node_to_selectPtr = vCanvasState->node_to_select.lock();

		//if (current_selected_nodePtr != node_to_selectPtr)
		{
			vCanvasState->current_selected_node = vCanvasState->node_to_select;

			if (!vCanvasState->current_selected_node.expired())
			{
				auto nodePtr = vCanvasState->current_selected_node.lock();
				if (nodePtr)
				{
					BlueprintManager::Instance()->SelectNodeForPreview(vCanvasState->current_selected_node);
				}
			}
			vCanvasState->node_to_select.reset();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
////// CLEAR /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::ClearNode()
{
	
}

void BaseNode::ClearGraph()
{
	m_ChildNodes.clear();
	m_Links.clear();
	m_NodeStamps.clear();
	m_LinksToBuildAfterLoading.clear();
}

void BaseNode::ClearSlots()
{
	m_Slots.clear();
}

void BaseNode::UpdateSlots()
{

}

void BaseNode::ClearDescriptors()
{

}

//////////////////////////////////////////////////////////////////////////////
////// GAPH NAVIGATION ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::ZoomToContent() const
{
	/*if (m_NodeGraphContext)
	{
		auto context = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(m_NodeGraphContext);
		context->NavigateTo(context->GetContentBounds(), true);
	}*/
}

void BaseNode::NavigateToContent() const
{
	/*if (m_NodeGraphContext)
	{
		auto context = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(m_NodeGraphContext);
		context->NavigateTo(context->GetContentBounds(), false);
	}*/
}

void BaseNode::ZoomToSelection() const
{
	/*if (m_NodeGraphContext)
	{
		auto context = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(m_NodeGraphContext);
		context->NavigateTo(context->GetSelectionBounds(), true);
	}*/
}

void BaseNode::NavigateToSelection() const
{
	/*if (m_NodeGraphContext)
	{
		auto context = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(m_NodeGraphContext);
		context->NavigateTo(context->GetSelectionBounds(), false);
	}*/
}

//////////////////////////////////////////////////////////////////////////////
////// CANVAS QUERY //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

ImVec2 BaseNode::GetCanvasOffset() const
{
	/*if (m_NodeGraphContext)
	{
		auto context = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(m_NodeGraphContext);
		return context->GetView().Origin;
	}*/

	return ImVec2(0, 0);
}

//////////////////////////////////////////////////////////////////////////////
////// COPY / PASTE / DUPLICATE //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::CopySelectedNodes()
{
	/*auto countSelectecdNodes = GetSelectedObjectCount();
	m_NodesToCopy.resize(countSelectecdNodes);
	GetActionContextNodes(m_NodesToCopy.data(), (int)m_NodesToCopy.size());

	// calcul du point de centre de tout ces nodes
	// sa servira d'offset avec le point de destinatiion
	m_BaseCopyOffset = ImVec2(0,0);
	for (auto id : m_NodesToCopy)
	{
		m_BaseCopyOffset += GetNodePosition(id) * 0.5f;
	}*/
}

void BaseNode::Suspend()
{
	auto drawList = ImGui::GetWindowDrawList();
	auto lastChannel = drawList->_Splitter._Current;
	drawList->ChannelsSetCurrent(m_ExternalChannel);
	m_Canvas.Suspend();
	drawList->ChannelsSetCurrent(lastChannel);
}

void BaseNode::Resume()
{
	auto drawList = ImGui::GetWindowDrawList();
	auto lastChannel = drawList->_Splitter._Current;
	drawList->ChannelsSetCurrent(m_ExternalChannel);
	m_Canvas.Resume();
	drawList->ChannelsSetCurrent(lastChannel);
}

void BaseNode::PasteNodesAtMousePos()
{
	Suspend(); // necessaire pour avoir le bon MousePos
	auto newOffset = m_Canvas.ToLocal(ImGui::GetMousePos()) - m_BaseCopyOffset;
	Resume();
	DuplicateSelectedNodes(newOffset);
}

void BaseNode::DuplicateSelectedNodes(ImVec2 vOffset)
{
	for (auto &it : m_NodesToCopy)
	{
		DuplicateNode((uint32_t)it, vOffset);
	}
	m_NodesToCopy.clear();
}

//////////////////////////////////////////////////////////////////////////////
////// NODE DRAWING //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::DrawNode(BaseNodeStateStruct *vCanvasState)
{
	if (vCanvasState)
	{
		if (DrawBegin(vCanvasState))
		{
			DrawHeader(vCanvasState);
			DrawNodeContent(vCanvasState);
			DrawFooter(vCanvasState);
			DrawEnd(vCanvasState);
		}
		size = ImGui::GetItemRectSize();
		pos = ImGui::GetItemRectMin();

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(0)) // bouton gauche click
			{
				assert(!m_This.expired());
				vCanvasState->node_to_select = m_This;
			}
		}
	}
}

void BaseNode::FillState(BaseNodeStateStruct *vCanvasState)
{
	vCanvasState->hoveredItem = ImGui::IsItemHovered();
	vCanvasState->activeItem = ImGui::IsItemActive();
	vCanvasState->is_any_hovered_items |= vCanvasState->hoveredItem;
	vCanvasState->is_any_active_items |= vCanvasState->activeItem;

	if (vCanvasState->hoveredItem)
	{
		assert(!m_This.expired());
		vCanvasState->current_hovered_node = m_This;
	}
	else
	{
		vCanvasState->current_hovered_node.reset();
		if (ImGui::IsMouseClicked(0)) // bouton gauche click
		{
			vCanvasState->node_to_select.reset();
			vCanvasState->current_selected_node.reset();
		}
	}

	if (vCanvasState->hoveredItem &&
		vCanvasState->activeItem)
	{
		assert(!m_This.expired());
		vCanvasState->current_selected_node = m_This;
	}
	else
	{

	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool BaseNode::DrawNodeContent(BaseNodeStateStruct *vCanvasState)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));

	/*ImGui::BeginHorizontal("content");
	ImGui::Spring(0, 0);

	ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);
	for (auto & param : m_Slots) // slots
	{
		param.second->DrawContent(vCanvasState);
	}
	ImGui::EndVertical();

	ImGui::Spring(1, 5.0f); // pour que BeginVertical soi poussé au bout

	ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f); // 1.0f pour que l'interieur soit aligné sur la fin
	
	ImGui::EndVertical();

	ImGui::EndHorizontal();*/

	ImGui::PopStyleVar();

	return true;
}

void BaseNode::DrawInputWidget(BaseNodeStateStruct *vCanvasState, std::weak_ptr<NodeSlot> vSlot)
{
	UNUSED(vCanvasState);
	UNUSED(vSlot);
}

void BaseNode::DrawOutputWidget(BaseNodeStateStruct *vCanvasState, std::weak_ptr<NodeSlot> vSlot)
{
	UNUSED(vCanvasState);
	UNUSED(vSlot);
}

void BaseNode::DrawContextMenuForSlot(BaseNodeStateStruct *vCanvasState, std::weak_ptr<NodeSlot> vSlot)
{
	UNUSED(vCanvasState);
	UNUSED(vSlot);
}

void BaseNode::DrawContextMenuForNode(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);
}

void BaseNode::DrawCustomContextMenuForNode(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);
}

std::string BaseNode::GetNodeCode(bool vRecursChilds)
{
	UNUSED(vRecursChilds);
	return "";
}

bool BaseNode::IsCodeDirty()
{
	return m_IsCodeDirty;
}

void BaseNode::SetCodeDirty(bool vFlag)
{
	m_IsCodeDirty = vFlag;
}

void BaseNode::JustConnectedBySlots(std::weak_ptr<NodeSlot> vStartSlot, std::weak_ptr<NodeSlot> vEndSlot)
{

}

void BaseNode::JustDisConnectedBySlots(std::weak_ptr<NodeSlot> vStartSlot, std::weak_ptr<NodeSlot> vEndSlot)
{

}

void BaseNode::CompilGeneratedCode()
{
	/*std::string code = GetNodeCode();
	if (!code.empty())
	{
		
	}*/
}

bool BaseNode::DrawBegin(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);

	ImGui::PushID(nodeID);
	ImGui::BeginGroup();

	auto drawList = ImGui::GetWindowDrawList();
	m_ExternalChannel = drawList->_Splitter._Current;

	//ImGui::BeginVertical("node");

	return true;
}

bool BaseNode::DrawHeader(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);

	/*ImGui::BeginHorizontal("header");
	ImGui::Spring(1, 5.0f);
	ImGui::TextUnformatted(name.c_str());
	ImGui::Spring(1, 5.0f);
	//ImGui::Dummy(ImVec2(0, 24));
	ImGui::EndHorizontal();*/

    m_HeaderRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

	return true;
}

bool BaseNode::DrawFooter(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);

	return true;
}

bool BaseNode::DrawEnd(BaseNodeStateStruct *vCanvasState)
{
	//ImGui::EndVertical();

	ImGui::EndGroup();

	if (ImGui::IsItemVisible())
	{
		auto drawList = ImGui::GetWindowDrawList();//GetNodeBackgroundDrawList(nodeID);
		if (drawList)
		{
			ImGuiContext& g = *GImGui;
			const auto itemRect = g.LastItemData.Rect;

			drawList->AddRectFilled(itemRect.Min, itemRect.Max, 
				ImGui::GetColorU32(ImVec4(0.2, 0.5, 0.2, 0.8)), 
				m_BaseNodeState.graphStyle.NodeRounding, ImDrawFlags_RoundCornersAll);

			/*if (m_HeaderRect.GetSize().y > 0.0f)
			{
				const ImVec4 NodePadding = m_BaseNodeState.graphStyle.NodePadding;
				const auto halfBorderWidth = 50.0f;

				auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);

				drawList->AddLine(
					ImVec2(m_HeaderRect.Min.x - (NodePadding.x - halfBorderWidth), m_HeaderRect.Max.y - 0.5f),
					ImVec2(m_HeaderRect.Max.x + (NodePadding.z - halfBorderWidth), m_HeaderRect.Max.y - 0.5f),
					ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
			}*/
			
			DisplayInfosOnTopOfTheNode(vCanvasState);
		}
		else
		{
			LogVarDebug("why drawList is null ?? in BaseNode::DrawEnd");
		}
	}

	ImGui::PopID();

	return true;
}

void BaseNode::DrawLinks(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);
	if (!m_BaseNodeState.showLinks) return;

	if (!m_ChildNodes.empty())
	{
		for (const auto &link : m_Links)
		{
			if (!link.second->in.expired() && 
				!link.second->out.expired())
			{
				auto inPtr = link.second->in.lock();
				auto outPtr = link.second->out.lock();
				if (inPtr && outPtr)
				{
					/*Link(
						link.first, inPtr->pinID, outPtr->pinID,
						link.second->color, link.second->thick);*/
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool BaseNode::DrawDebugInfos(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);
	ImGui::Separator();

	/*auto rend = GetRenderer();
	if (!rend.expired())
	{
		auto rendPtr = rend.lock();
		if (rendPtr)
		{
			ImGui::Text("CodeTree");

			//rendPtr->DisplayCodeTree();
		}
	}*/

	ImGui::Text("Code Generator");

	ImGui::Separator();
	
	if (!m_ParentNode.expired())
	{
		auto parentPtr = m_ParentNode.lock();
		if (parentPtr)
		{
			ImGui::Separator();
			ImGui::Text("Parent Code");
			ImGui::Indent();
			//ImGui::Text("Func Code : %s", parentPtr->m_Code.m_MainCode.c_str());
			ImGui::Unindent();
			ImGui::Separator();
		}
	}

	ImGui::Text("Name : %s", name.c_str());
	ImGui::Text("Type : %s", GetStringFromNodeTypeEnum(m_NodeType).c_str());
	//ImGui::Text("Func Code : %s", this->m_Code.m_MainCode.c_str());

	ImGui::Separator();

	if (!m_Slots.empty())
	{
		if (ImGui::TreeNode("Input Calls :"))
		{
			ImGui::Indent();
			for (auto & uni : m_Slots) // input calls
			{
				ImGui::Text("Name : %s", uni.second->name.c_str());
				uni.second->stamp.DrawImGui();
				ImGui::Text("Type : %s", NodeSlot::GetStringFromNodeSlotTypeEnum(uni.second->slotType).c_str());
				ImGui::Text("Func Code : %s", uni.second->funcCode.c_str());
				ImGui::Separator();
			}
			ImGui::Unindent();

			ImGui::TreePop();
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::DisplayInfosOnTopOfTheNode(BaseNodeStateStruct *vCanvasState)
{
	if (vCanvasState && vCanvasState->debug_mode)
	{
		auto drawList = ImGui::GetWindowDrawList();//GetNodeBackgroundDrawList(nodeID);
		if (drawList)
		{
			char debugBuffer[255] = "\0";
			snprintf(debugBuffer, 254, 
				"Used(%s)\nCell(%i, %i)"/*\nPos(%.1f, %.1f)\nSize(%.1f, %.1f)*/, 
				(used?"true":"false"), cell.x, cell.y/*, pos.x, pos.y, size.x, size.y*/);
			ImVec2 txtSize = ImGui::CalcTextSize(debugBuffer);
			drawList->AddText(pos - ImVec2(0, txtSize.y), ImGui::GetColorU32(ImGuiCol_Text), debugBuffer);
		}
	}
}

void BaseNode::DrawProperties(BaseNodeStateStruct* vCanvasState)
{
	UNUSED(vCanvasState);
	ImGui::Text("Properties of %s", name.c_str());
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::weak_ptr<NodeSlot> BaseNode::AddSlot(NodeSlot vSlot, bool vIncSlotId, bool vHideName)
{
	assert(!m_This.expired());
	vSlot.parentNode = m_This;
	vSlot.stamp.typeStamp = "flow";
	vSlot.slotPlace = NodeSlot::NodeSlotPlaceEnum::NODE_SLOT_PLACE_INPUT;
	vSlot.slotType = NodeSlot::NodeSlotTypeEnum::NODE_SLOT_TYPE_DEFAULT;
	vSlot.hideName = vHideName;
	vSlot.type = uType::uTypeEnum::U_FLOW;
	if (vIncSlotId)
		vSlot.pinID = NodeSlot::GetNewSlotId();
	m_Slots[(int)vSlot.pinID] = std::make_shared<NodeSlot>(vSlot);
	m_Slots[(int)vSlot.pinID]->m_This = m_Slots[(int)vSlot.pinID];
	return m_Slots[(int)vSlot.pinID];
}

//////////////////////////////////////////////////////////////////////////////
////// NODE STYLE ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::DrawStyleMenu()
{
	/*static GraphStyleStruct graphStyleDefault = GraphStyleStruct();

	if (ImGui::BeginMenu("Node"))
	{
		ImGui::SliderFloatDefault(200, "Width", &m_BaseNodeState.graphStyle.NODE_DEFAULT_WIDTH, 1.0f,
			graphStyleDefault.NODE_DEFAULT_WIDTH*3.0f, graphStyleDefault.NODE_DEFAULT_WIDTH);
		ImGui::SliderFloatDefault(200, "Padding", &m_BaseNodeState.graphStyle.NODE_WINDOW_PADDING, 1.0f,
			graphStyleDefault.NODE_WINDOW_PADDING*3.0f, graphStyleDefault.NODE_DEFAULT_WIDTH);
		ImGui::SliderFloatDefault(200, "Radius", &m_BaseNodeState.graphStyle.NODE_BACKGROUND_RADIUS, 1.0f,
			20.0, graphStyleDefault.NODE_BACKGROUND_RADIUS);
		ImGui::ColorEdit4Default(200, "Header", &m_BaseNodeState.graphStyle.NODE_HEADER_COLOR.x, 
			&graphStyleDefault.NODE_HEADER_COLOR.x);
		ImGui::ColorEdit4Default(200, "Header Hovered", &m_BaseNodeState.graphStyle.NODE_HOVERED_HEADER_COLOR.x, 
			&graphStyleDefault.NODE_HOVERED_HEADER_COLOR.x);
		ImGui::ColorEdit4Default(200, "Background", &m_BaseNodeState.graphStyle.NODE_BACKGROUND_COLOR.x, 
			&graphStyleDefault.NODE_BACKGROUND_COLOR.x);
		ImGui::ColorEdit4Default(200, "Background Hovered", &m_BaseNodeState.graphStyle.NODE_HOVERED_BACKGROUND_COLOR.x, 
			&graphStyleDefault.NODE_HOVERED_BACKGROUND_COLOR.x);
		
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Slot"))
	{
		ImGui::SliderFloatDefault(200, "Radius", &m_BaseNodeState.graphStyle.NODE_SLOT_RADIUS, 1.0f,
			graphStyleDefault.NODE_SLOT_RADIUS*3.0f, graphStyleDefault.NODE_SLOT_RADIUS);
		ImGui::ColorEdit4Default(200, "Color", &m_BaseNodeState.graphStyle.NODE_SLOT_COLOR.x,
			&graphStyleDefault.NODE_SLOT_COLOR.x);
		ImGui::ColorEdit4Default(200, "Flow Color", &m_BaseNodeState.graphStyle.NODE_FLOW_SLOT_COLOR.x,
			&graphStyleDefault.NODE_FLOW_SLOT_COLOR.x);
		ImGui::ColorEdit4Default(200, "Function Color", &m_BaseNodeState.graphStyle.NODE_FLOW_SLOT_COLOR.x,
			&graphStyleDefault.NODE_FLOW_SLOT_COLOR.x);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Grid"))
	{
		ImGui::ColorEdit4Default(200, "Grid Lines", &m_BaseNodeState.graphStyle.GRID_COLOR.x, 
			&graphStyleDefault.GRID_COLOR.x);
		ImGui::ColorEdit4Default(200, "Grid Zero x", &m_BaseNodeState.graphStyle.GRID_COLOR_ZERO_X.x, 
			&graphStyleDefault.GRID_COLOR_ZERO_X.x);
		ImGui::ColorEdit4Default(200, "Grid Zero y", &m_BaseNodeState.graphStyle.GRID_COLOR_ZERO_Y.x, 
			&graphStyleDefault.GRID_COLOR_ZERO_Y.x);
		ImGui::SliderFloatDefault(200, "Grid Spacing", &m_BaseNodeState.graphStyle.GRID_SPACING, 1.0f,
			256, graphStyleDefault.GRID_SPACING);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Link"))
	{
		ImGui::ColorEdit4Default(200, "link", &m_BaseNodeState.graphStyle.linkDefaultColor.x,
			&graphStyleDefault.linkDefaultColor.x);
		ImGui::ColorEdit4Default(200, "selected Link", &m_BaseNodeState.graphStyle.selectedLinkColor.x,
			&graphStyleDefault.selectedLinkColor.x);
		ImGui::ColorEdit4Default(200, "extract Link", &m_BaseNodeState.graphStyle.extractionLinkColor.x,
			&graphStyleDefault.extractionLinkColor.x);
		ImGui::ColorEdit4Default(200, "selected Extract Link", &m_BaseNodeState.graphStyle.selectedExtractionLinkColor.x,
			&graphStyleDefault.selectedExtractionLinkColor.x);

		ImGui::EndMenu();
	}

	ImGui::MenuItem("Editor", "", &showNodeStyleEditor);*/
}

void BaseNode::DrawNodeGraphStyleMenu() const
{
	/*if (m_NodeGraphContext && showNodeStyleEditor)
	{
		if (ImGui::Begin("Editor", &showNodeStyleEditor))
		{
			SetCurrentEditor(m_NodeGraphContext);

			ImGui::SliderFloatDefault(200, "Node Padding x", &editorStyle.NodePadding.x, 0.0f, 40.0f, def.NodePadding.x);
			ImGui::SliderFloatDefault(200, "Node Padding y", &editorStyle.NodePadding.y, 0.0f, 40.0f, def.NodePadding.y);
			ImGui::SliderFloatDefault(200, "Node Padding z", &editorStyle.NodePadding.z, 0.0f, 40.0f, def.NodePadding.z);
			ImGui::SliderFloatDefault(200, "Node Padding w", &editorStyle.NodePadding.w, 0.0f, 40.0f, def.NodePadding.w);
			ImGui::SliderFloatDefault(200, "Node Rounding", &editorStyle.NodeRounding, 0.0f, 40.0f, def.NodeRounding);
			ImGui::SliderFloatDefault(200, "Node Border Width", &editorStyle.NodeBorderWidth, 0.0f, 15.0f, def.NodeBorderWidth);
			ImGui::SliderFloatDefault(200, "Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.0f, 15.0f, def.HoveredNodeBorderWidth);
			ImGui::SliderFloatDefault(200, "Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.0f, 15.0f, def.SelectedNodeBorderWidth);
			ImGui::SliderFloatDefault(200, "Slot Rounding", &editorStyle.PinRounding, 0.0f, 40.0f, def.PinRounding);
			ImGui::SliderFloatDefault(200, "Slot Border Width", &editorStyle.PinBorderWidth, 0.0f, 15.0f, def.PinBorderWidth);
			ImGui::SliderFloatDefault(200, "Link Strength", &editorStyle.LinkStrength, 0.0f, 500.0f, def.LinkStrength);

			//ImVec2 SourceDirection;
			//ImVec2 TargetDirection;
			ImGui::SliderFloatDefault(200, "Scroll Duration", &editorStyle.ScrollDuration, 0.0f, 2.0f, def.ScrollDuration);
			ImGui::SliderFloatDefault(200, "Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 200.0f, def.FlowMarkerDistance);
			ImGui::SliderFloatDefault(200, "Flow Speed", &editorStyle.FlowSpeed, 1.0f, 2000.0f, def.FlowSpeed);
			ImGui::SliderFloatDefault(200, "Flow Duration", &editorStyle.FlowDuration, 0.0f, 5.0f, def.FlowDuration);

			//ImVec2 PivotAlignment;
			//ImVec2 PivotSize;
			//ImVec2 PivotScale;
			//float SlotCorners;
			//float SlotRadius;
			//float SlotArrowSize;
			//float SlotArrowWidth;
			ImGui::SliderFloatDefault(200, "Group Rounding", &editorStyle.GroupRounding, 0.0f, 40.0f, def.GroupRounding);
			ImGui::SliderFloatDefault(200, "Group Border Width", &editorStyle.GroupBorderWidth, 0.0f, 15.0f, def.GroupBorderWidth);

			//ImGui::EndMenu();
		}
		ImGui::End();
	}*/
}

void BaseNode::DrawToolMenu()
{
	if (ImGui::BeginMenu("Tools"))
	{
		ImGui::CheckBoxBoolDefault("Debug mode", &m_BaseNodeState.debug_mode, false);

		if (ImGui::MenuItem("Show in Debug Pane"))
		{
			assert(!m_This.expired());
			assert(0);
			//DebugPane::Instance()->SelectNode(m_This);
		}

		ImGui::EndMenu();
	}
}

//////////////////////////////////////////////////////////////////////////////
////// NODE GRAPH ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::ResetActions()
{
	m_RequestContextMenu_Background = false;
}

void BaseNode::ManageContextMenus()
{
	const auto isClicked = ImGui::IsMouseClicked(1);

	if (isClicked && !m_IsDragging)
	{
		m_RequestContextMenu_Background = true;
	}
}

void BaseNode::ManageZoom()
{
	const auto& viewOrigin = m_Canvas.ViewOrigin();
	const auto& viewRect = m_Canvas.ViewRect();
	auto viewScale = m_Canvas.ViewScale();

	const auto& io = ImGui::GetIO();
	auto mousePos = io.MousePos;

	auto steps = (int)io.MouseWheel;
	if (IS_FLOAT_DIFFERENT(steps, 0.0f))
	{
		auto oldView = ImGuiEx::CanvasView(viewOrigin, viewScale);
		viewScale = ImMax(viewScale + steps * m_BaseNodeState.graphStyle.ZoomSpeed, 0.1f);
		auto newView = ImGuiEx::CanvasView(viewOrigin, viewScale);

		auto screenPos = m_Canvas.FromLocal(mousePos, oldView);
		auto canvasPos = m_Canvas.ToLocal(screenPos, newView);

		auto offset = (canvasPos - mousePos) * viewScale;
		auto targetScroll = viewOrigin + offset;

		m_Canvas.SetView(targetScroll, viewScale);
	}
}

void BaseNode::ManageDragging()
{
	const auto& viewOrigin = m_Canvas.ViewOrigin();
	const auto viewScale = m_Canvas.ViewScale();
	
	if ((m_IsDragging || ImGui::IsItemHovered()) && ImGui::IsMouseDragging(0, 0.0f))
	{
		if (!m_IsDragging)
		{
			m_IsDragging = true;
			m_DrawStartPoint = viewOrigin;
		}

		m_Canvas.SetView(m_DrawStartPoint + ImGui::GetMouseDragDelta(0, 0.0f) * viewScale, viewScale);
	}
	else if (m_IsDragging)
	{
		m_IsDragging = false;
	}
}

void BaseNode::ManageActions()
{
	ResetActions();

	if (ImGui::IsWindowHovered() && ImGui::IsWindowFocused())
	{
		ManageContextMenus();
		ManageZoom();
		ManageDragging();
	}
}

void BaseNode::DrawGridAndBackground()
{
	const auto  drawList = ImGui::GetWindowDrawList();

	//drawList->ChannelsSetCurrent(c_UserChannel_Grid);

	const ImVec2 offset = m_Canvas.ViewOrigin() * (1.0f / m_Canvas.ViewScale());
	const auto color = ImVec4(0.5, 0.5, 0.2, 1.0);
	const auto gridColor = m_BaseNodeState.graphStyle.Colors[StyleColor_Grid];
	const ImU32 GRID_COLOR = ImColor(gridColor.x, gridColor.y, gridColor.z, gridColor.w * ImClamp(m_Canvas.ViewScale() * m_Canvas.ViewScale(), 0.0f, 1.0f));
	const float GRID_SX = 32.0f;// * m_Canvas.ViewScale();
	const float GRID_SY = 32.0f;// * m_Canvas.ViewScale();
	const ImVec2 VIEW_POS = m_Canvas.ViewRect().Min;
	const ImVec2 VIEW_SIZE = m_Canvas.ViewRect().GetSize();

	drawList->AddRectFilled(VIEW_POS, VIEW_POS + VIEW_SIZE, ImColor(m_BaseNodeState.graphStyle.Colors[StyleColor_Bg]));

	for (float x = fmodf(offset.x, GRID_SX); x < VIEW_SIZE.x; x += GRID_SX)
		drawList->AddLine(ImVec2(x, 0.0f) + VIEW_POS, ImVec2(x, VIEW_SIZE.y) + VIEW_POS, GRID_COLOR);
	for (float y = fmodf(offset.y, GRID_SY); y < VIEW_SIZE.y; y += GRID_SY)
		drawList->AddLine(ImVec2(0.0f, y) + VIEW_POS, ImVec2(VIEW_SIZE.x, y) + VIEW_POS, GRID_COLOR);
}

bool BaseNode::DrawGraph()
{
	bool change = false;

	auto availableContentSize = ImGui::GetContentRegionAvail();
	if (m_Canvas.Begin("canvas", availableContentSize))
	{
		ManageActions();
		DrawGridAndBackground();
		
		// draw nodes
		m_BaseNodeState.itemPushId = 1;
		if (!m_ChildNodes.empty())
		{
			for (auto& node : m_ChildNodes)
			{
				node.second->DrawNode(&m_BaseNodeState);
			}

			DrawLinks(&m_BaseNodeState);

			DoCreateLinkOrNode(&m_BaseNodeState);
			DoDeleteLinkOrNode(&m_BaseNodeState);
			DoShorcutsOnNode(&m_BaseNodeState);
		}

		DoPopups(&m_BaseNodeState);

		m_Canvas.End();

		DoGraphActions(&m_BaseNodeState);
	}
	
	return change;
}

bool BaseNode::GenerateGraphFromCode(const std::string& vCode)
{
	assert(!m_This.expired());
	//GraphGenerator::Instance()->GenerateGraphFromCodeForHostNode(vCode, m_This);

	return true;
}

std::weak_ptr<BaseNode> BaseNode::FindNode(uint32_t vQueryId)
{
	std::weak_ptr<BaseNode> res;

	if (m_ChildNodes.find(vQueryId) != m_ChildNodes.end()) // trouvé
	{
		if (m_ChildNodes[vQueryId]->nodeID == vQueryId)
		{
			res = m_ChildNodes[vQueryId];
		}
		else
		{
			uint32_t nodeId = (uint32_t)m_ChildNodes[vQueryId]->nodeID;
			LogVarDebug("Comment c'est possible que le m_ChildNodes avec id %u dans la map ait un autre indice %u", vQueryId, nodeId);
		}
	}
		
	return res;
}

std::weak_ptr<BaseNode> BaseNode::FindNodeByName(std::string vName)
{
	std::weak_ptr<BaseNode> res;

	for (auto node : m_ChildNodes)
	{
		if (node.second)
		{
			if (node.second->name == vName)
			{
				res = node.second;
				break;
			}
		}
	}

	return res;
}

std::vector<std::weak_ptr<BaseNode>> BaseNode::GetPublicNodes()
{
	std::vector<std::weak_ptr<BaseNode>> res;

	for (auto node : m_ChildNodes)
	{
		if (node.second)
		{
			if (node.second->name.find("PUBLIC_") == 0)
			{
				res.push_back(node.second);
			}
		}
	}

	return res;
}

std::weak_ptr<NodeLink> BaseNode::FindLink(uint32_t vId)
{
	UNUSED(vId);

	for (auto & link : m_Links)
	{
		if (link.second->linkId == vId)
		{
			return link.second;
		}
	}

	return std::weak_ptr<NodeLink>();
}

std::weak_ptr<NodeSlot> BaseNode::FindSlot(uint32_t vId)
{
	if (vId)
	{
		for (auto & node : m_ChildNodes)
		{
			for (auto& pin : node.second->m_Slots)
				if (pin.second->pinID == vId)
					return pin.second;
		}
	}
	
	return std::weak_ptr<NodeSlot>();
}

std::weak_ptr<NodeSlot> BaseNode::FindNodeSlotByName(std::weak_ptr<BaseNode> vNode, std::string vName)
{
	if (!vNode.expired())
	{
		auto nodePtr = vNode.lock();
		if (nodePtr)
		{
			for (auto& pin : nodePtr->m_Slots)
				if (pin.second->name == vName)
					return pin.second;
		}
	}

	return std::weak_ptr<NodeSlot>();
}

void BaseNode::DoCreateLinkOrNode(BaseNodeStateStruct *vCanvasState)
{
	/*if (BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		auto showLabel = [](const char* label, ImColor color)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
			auto _size_ = ImGui::CalcTextSize(label);

			auto padding = ImGui::GetStyle().FramePadding;
			auto spacing = ImGui::GetStyle().ItemSpacing;

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

			auto rectMin = ImGui::GetCursorScreenPos() - padding;
			auto rectMax = ImGui::GetCursorScreenPos() + _size_ + padding;

			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(rectMin, rectMax, color, _size_.y * 0.15f);
			ImGui::TextUnformatted(label);
		};

		// new link
		uint32_t startSlotId = 0, endSlotId = 0;
		if (QueryNewLink(&startSlotId, &endSlotId))
		{
			auto startSlot = FindSlot(startSlotId);
			auto endSlot = FindSlot(endSlotId);

			if (!startSlot.expired())
			{
				auto startSlotPtr = startSlot.lock();
				if (startSlotPtr)
				{
					if (!endSlot.expired())
					{
						auto endSlotPtr = endSlot.lock();
						if (endSlotPtr)
						{
							if (startSlotPtr == endSlotPtr)
							{
								RejectNewItem(ImColor(255, 0, 0), 2.0f);
							}
							else if (startSlotPtr->slotPlace == endSlotPtr->slotPlace)
							{
								showLabel("x Incompatible Slot Kind", ImColor(45, 32, 32, 180));
								RejectNewItem(ImColor(255, 0, 0), 2.0f);
							}
							//else if (endSlot->parentNode == startSlot->parentNode) // desactivé pour les self connect en compute et fragment effect
							//{
							//	showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
							//	RejectNewItem(ImColor(255, 0, 0), 1.0f);
							//}
							else if (!startSlotPtr->CanWeConnectToSlot(endSlot) ||
							!endSlotPtr->CanWeConnectToSlot(startSlot)) // si un des deux est pas d'accord pour ken on ce barre
							{
							showLabel("x Incompatible Slot Type", ImColor(45, 32, 32, 180));
							RejectNewItem(ImColor(255, 128, 128), 1.0f);
							}
							else
							{
							showLabel("+ Create Link", ImColor(32, 45, 32, 180));
							if (AcceptNewItem(ImColor(128, 255, 128), 4.0f))
							{
								ConnectSlots(startSlotPtr, endSlotPtr);
							}
						}
						}
					}
				}
			}
		}

		// new node
		uint32_t slotId = 0;
		if (QueryNewNode(&slotId))
		{
			auto newNodeLinkSlot = FindSlot(slotId);
			if (!newNodeLinkSlot.expired())
			{
				auto newLinkSlotPtr = newNodeLinkSlot.lock();
				if (newLinkSlotPtr)
				{
					if (newLinkSlotPtr)
						showLabel("Add node", ImColor(32, 45, 32, 180));
				}
			}

			if (AcceptNewItem())
			{
				vCanvasState->linkFromSlot = newNodeLinkSlot;
				m_CreateNewNode = true;
				Suspend();
				ImGui::OpenPopup("CreateNewNode");
				Resume();
			}
		}
	}
	EndCreate();*/
}

void BaseNode::DoDeleteLinkOrNode(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);

	/*if (BeginDelete())
	{
		bool canDeleteLinks = true;
		uint32_t nodeId = 0;
		while (QueryDeletedNode(&nodeId))
		{
			if (AcceptDeletedItem())
			{
				if (!DestroyChildNodeByIdIfAllowed((int)nodeId.Get(), false))
				{
					canDeleteLinks = false;
				}
			}
		}

		if (canDeleteLinks)
		{
			uint32_t linkId = 0;
			while (QueryDeletedLink(&linkId))
			{
				if (AcceptDeletedItem())
				{
					auto id = (int)linkId.Get();
					if (m_Links.find(id) != m_Links.end())
					{
						if (DisConnectSlots(m_Links[id]->in, m_Links[id]->out))
						{
							Del_VisualLink(id);
						}
					}
				}
			}
		}
	}

	EndDelete();*/
}

void BaseNode::DoShorcutsOnNode(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);

	/*if (BeginShortcut())
	{
		if (AcceptCopy())
		{
			CopySelectedNodes();
		}

		if (AcceptPaste())
		{
			PasteNodesAtMousePos();
		}

		if (AcceptDuplicate())
		{
			//DuplicateSelectedNodes();
		}
	}

	EndShortcut();*/
}

bool BaseNode::ShowNodeContextMenu(uint32_t *vNodeId)
{
	return (m_RequestContextMenu_NodeId != 0);
}

bool BaseNode::ShowPinContextMenu(uint32_t *vPinId)
{
	return (m_RequestContextMenu_PinId != 0);
}

bool BaseNode::ShowLinkContextMenu(uint32_t *vLinkId)
{
	return (m_RequestContextMenu_LinkId != 0);
}

bool BaseNode::ShowBackgroundContextMenu()
{
	return m_RequestContextMenu_Background;
}

void BaseNode::DoPopups(BaseNodeStateStruct *vCanvasState)
{
	m_OpenPopupPosition = ImGui::GetMousePos();

	Suspend();

	if (vCanvasState->m_CustomContextMenuRequested &&
		!vCanvasState->m_CustomContextMenuNode.expired())
	{
		ImGui::OpenPopup("CustomNodePopup");
		vCanvasState->m_CustomContextMenuRequested = false;
	}
	else if (ShowNodeContextMenu(&m_ContextMenuNodeId))
	{
		ImGui::OpenPopup("NodeContextMenu");
	}
	else if (ShowPinContextMenu(&m_ContextMenuSlotId))
	{
		ImGui::OpenPopup("SlotContextMenu");
	}
	else if (ShowLinkContextMenu(&m_ContextMenuLinkId))
	{
		ImGui::OpenPopup("LinkContextMenu");
	}
	else if (ShowBackgroundContextMenu())
	{
		vCanvasState->linkFromSlot.reset();
		ImGui::OpenPopup("CreateNewNode");
	}

	DoCheckNodePopup(vCanvasState);
	DoCheckSlotPopup(vCanvasState);
	DoCheckLinkPopup(vCanvasState);
	DoNewNodePopup(vCanvasState);

	Resume();
}

void BaseNode::DoCheckNodePopup(BaseNodeStateStruct *vCanvasState)
{
	if (ImGui::BeginPopup("CustomNodePopup"))
	{
		if (!vCanvasState->m_CustomContextMenuNode.expired())
		{
			auto nodePtr = vCanvasState->m_CustomContextMenuNode.lock();
			if (nodePtr)
			{
				nodePtr->DrawCustomContextMenuForNode(vCanvasState);
			}
		}
		
		ImGui::EndPopup();
	}
	/*else
	{
		vCanvasState->m_CustomContextMenuNode.reset();

		if (ImGui::BeginPopup("NodeContextMenu"))
		{
			auto node = FindNode(m_ContextMenuNodeId);
			if (!node.expired())
			{
				auto nodePtr = node.lock();
				if (nodePtr)
				{
					nodePtr->DrawContextMenuForNode(vCanvasState);
				}
				else
				{
					ImGui::Text("cant lock node: %p", m_ContextMenuNodeId.AsPointer());
				}
				ImGui::Separator();
			}
			else
			{
				ImGui::Text("Unknown node: %p", m_ContextMenuNodeId.AsPointer());
				ImGui::Separator();
			}
			
			if (ImGui::MenuItem("Delete"))
			{
				DeleteNode(m_ContextMenuNodeId);
			}

			ImGui::EndPopup();
		}
	}*/
}

void BaseNode::DoCheckSlotPopup(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);

	if (ImGui::BeginPopup("SlotContextMenu"))
	{
		auto slot = FindSlot(m_ContextMenuSlotId);
		if (!slot.expired())
		{
			auto slotPtr = slot.lock();
			if (slotPtr)
			{
				if (!slotPtr->parentNode.expired())
				{
					auto nodePtr = slotPtr->parentNode.lock();
					if (nodePtr)
					{
						nodePtr->DrawContextMenuForSlot(vCanvasState, slot);
					}
				}
			}
		}
		else
		{
			ImGui::Text("Unknown Slot: %p", m_ContextMenuSlotId);
		}
		ImGui::EndPopup();
	}
}

void BaseNode::DoCheckLinkPopup(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);

	if (ImGui::BeginPopup("LinkContextMenu"))
	{
		auto link = FindLink(m_ContextMenuLinkId);
		if (link.expired())
		{
			ImGui::Text("Unknown link: %p", m_ContextMenuLinkId);
		}
		ImGui::Separator();
		//if (ImGui::MenuItem("Delete"))
		//	DeleteLink(m_ContextMenuLinkId);
		ImGui::EndPopup();
	}
}

void BaseNode::DoNewNodePopup(BaseNodeStateStruct *vCanvasState)
{
	UNUSED(vCanvasState);

	assert(!m_This.expired());

	//if (m_Depth == 0)
	{
		UserNodeLibrary::Instance()->ShowNewNodeMenu(m_This, vCanvasState);
	}
	//else
	{
		//NodeLibrary::Instance()->ShowNewNodeMenu(NodeLibrary::NodeLibraryTypeEnum::NODE_LIBRARY_TYPE_BLUEPRINT, m_This);
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void BaseNode::DuplicateNode(uint32_t vNodeId, ImVec2 vOffsetPos)
{
	uint32_t nodeId = vNodeId;
	auto foundNode = FindNode(nodeId);
	if (!foundNode.expired())
	{
		auto foundNodePtr = foundNode.lock();
		if (foundNodePtr)
		{
			std::weak_ptr<BaseNode> createdNode;

			/*if (foundNodePtr->m_NodeType == NodeTypeEnum::NODE_TYPE_COMPUTE_EFFECT)
			{
				auto newNode = ComputeNode::Create(foundNodePtr->GetNodeCode());
				if (newNode)
				{
					newNode->name = foundNodePtr->name;
					newNode->pos = foundNodePtr->pos;
					createdNode = AddChildNode(newNode, true);
				}
			}
			else if (foundNodePtr->m_NodeType == NodeTypeEnum::NODE_TYPE_FRAGMENT_EFFECT)
			{
				auto newNode = FragmentNode::Create(foundNodePtr->GetNodeCode());
				if (newNode)
				{
					newNode->name = foundNodePtr->name;
					newNode->pos = foundNodePtr->pos;
					createdNode = AddChildNode(newNode, true);
				}
			}*/

			if (!createdNode.expired())
			{
				auto createdNodePtr = createdNode.lock();
				if (createdNodePtr)
				{
					/*SetNodePosition(
						createdNodePtr->nodeID,
						createdNodePtr->pos + vOffsetPos);*/
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::string BaseNode::GetAvailableNodeStamp(const std::string& vNodeStamp)
{
	std::string res;

	if (m_NodeStamps.find(vNodeStamp) != m_NodeStamps.end()) // trouv�
	{
		m_NodeStamps[vNodeStamp]++;
		res = vNodeStamp + "_" + ct::toStr(m_NodeStamps[vNodeStamp]);
	}
	else
	{
		m_NodeStamps[vNodeStamp] = 0;
		res = vNodeStamp;
	}

	return res;
}

std::weak_ptr<BaseNode> BaseNode::AddChildNode(std::shared_ptr<BaseNode> vNode, bool vIncNodeId)
{
	if (vNode)
	{
		assert(!m_This.expired());
		vNode->m_ParentNode = m_This;

		if (vIncNodeId)
			vNode->nodeID = GetNextNodeId();

		m_ChildNodes[(int)vNode->nodeID] = vNode;

		return vNode;
	}

	return std::weak_ptr<BaseNode>();
}

std::shared_ptr<BaseNode> BaseNode::GetRootNodeBySlots(std::shared_ptr<BaseNode> vNode) // get the root node by exploring tree with out call slots
{
	std::shared_ptr<BaseNode> res = vNode;

	if (!vNode)
	{
		assert(!m_This.expired());
		if (!m_This.expired())
		{
			auto nodePtr = m_This.lock();
			if (nodePtr)
			{
				res = nodePtr;
			}
		}
	}

	if (res)
	{
		if (res->m_Slots.size() == 1U)
		{
			auto call = res->m_Slots[0];
			if (call)
			{
				if (!call->parentNode.expired())
				{
					auto nodePtr = call->parentNode.lock();
					if (nodePtr)
					{
						res = GetRootNodeBySlots(nodePtr);
					}
				}
			}
		}
	}

	return res;
}

void BaseNode::DestroyChildNode(std::weak_ptr<BaseNode> vNode)
{
	if (!vNode.expired())
	{
		auto nodePtr = vNode.lock();
		if (nodePtr)
		{
			int nid = (int)nodePtr->nodeID;
			if (m_ChildNodes.find(nid) != m_ChildNodes.end()) // trouvé
			{
				m_ChildNodes.erase(nid);
			}
		}
	}
}

bool BaseNode::DestroyChildNodeByIdIfAllowed(int vNodeID, bool vDestroy)
{
	bool res = false;

	if (m_ChildNodes.find(vNodeID) != m_ChildNodes.end()) // trouvé
	{
		if (!m_ChildNodes[vNodeID]->deletionDisabled)
		{
			if (vDestroy)
			{
				m_ChildNodes.erase(vNodeID);
			}
			else
			{
				m_NodeIdToDelete.emplace(vNodeID);
			}

			res = true;
		}
	}

	return res;
}

void BaseNode::DestroySlotOfAnyMap(std::weak_ptr<NodeSlot> vSlot)
{
	if (!vSlot.expired())
	{
		auto slotPtr = vSlot.lock();
		if (slotPtr)
		{
			int sid = (int)slotPtr->pinID;

			Break_VisualLinks_ConnectedToSlot(vSlot);

			switch (slotPtr->slotType)
			{
			case NodeSlot::NodeSlotTypeEnum::NODE_SLOT_TYPE_NONE:
				break;
			case NodeSlot::NodeSlotTypeEnum::NODE_SLOT_TYPE_DEFAULT:
				if (m_Slots.find(sid) != m_Slots.end())
					m_Slots.erase(sid);
				break;
			}
		}
	}
}

// a executer apres le rendu de imgui
void BaseNode::DestroyNodesIfAnys()
{
	for (auto & nodeId : m_NodeIdToDelete)
	{
		DestroyChildNodeByIdIfAllowed(nodeId, true);
	}
	m_NodeIdToDelete.clear();

	// check for sub graphs
	for (auto & node : m_ChildNodes)
	{
		node.second->DestroyNodesIfAnys();
	}
}

void BaseNode::SetChanged(bool vFlag)
{
	changed = vFlag;
	if (!m_ParentNode.expired())
	{
		auto ptr = m_ParentNode.lock();
		if (ptr && ptr.use_count())
		{
			ptr->SetChanged(vFlag);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// LAYOUT //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void BaseNode::DoLayout()
{
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// GET LINKS / SLOTS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::weak_ptr<NodeLink>> BaseNode::GetLinksAssociatedToSlot(std::weak_ptr<NodeSlot> vSlot)
{
	std::vector<std::weak_ptr<NodeLink>> res;

	if (!vSlot.expired())
	{
		auto slotPtr = vSlot.lock();
		if (slotPtr)
		{
			uint32_t pinId = (uint32_t)slotPtr->pinID;

			if (m_LinksDico.find(pinId) != m_LinksDico.end()) // trouve
			{
				auto linkIds = m_LinksDico[pinId]; // link Ptr pointe sur une entrée de m_Links
				for (auto lid : linkIds)
				{
					if (m_Links.find(lid) != m_Links.end())
					{
						res.push_back(m_Links[lid]);
					}
				}
			}
		}
	}

	return res;
}

std::vector<std::weak_ptr<NodeSlot>> BaseNode::GetSlotsAssociatedToSlot(std::weak_ptr<NodeSlot> vSlot)
{
	std::vector<std::weak_ptr<NodeSlot>> res;

	if (!vSlot.expired())
	{
		auto slotPtr = vSlot.lock();
		if (slotPtr)
		{
			res = slotPtr->links;
		}
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// ADD/DELETE VISUAL LINKS (NO CHANGE BEHIND) //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void BaseNode::Add_VisualLink(std::weak_ptr<NodeSlot> vStart, std::weak_ptr<NodeSlot> vEnd)
{
	if (!vStart.expired() && 
		!vEnd.expired())
	{
		auto startPtr = vStart.lock();
		auto endPtr = vEnd.lock();

		if (startPtr && endPtr)
		{
			startPtr->connected = true;
			endPtr->connected = true;

			startPtr->links.push_back(vEnd);
			endPtr->links.push_back(vStart);

			NodeLink link;
			link.in = vStart;
			link.out = vEnd;
			link.linkId = GetNextNodeId();

			m_Links[link.linkId] = std::make_shared<NodeLink>(link);
			m_LinksDico[(uint32_t)startPtr->pinID].emplace(m_Links[link.linkId]->linkId);
			m_LinksDico[(uint32_t)endPtr->pinID].emplace(m_Links[link.linkId]->linkId);
		}
	}
}

void BaseNode::Del_VisualLink(uint32_t vLinkId)
{
	if (m_Links.find(vLinkId) != m_Links.end())
	{
		if (!m_Links[vLinkId]->in.expired() &&
			!m_Links[vLinkId]->out.expired())
		{
			auto inPtr = m_Links[vLinkId]->in.lock();
			auto outPtr = m_Links[vLinkId]->out.lock();

			if (inPtr && outPtr)
			{
				inPtr->links.clear();
				inPtr->connected = false;
				m_LinksDico.erase((uint32_t)inPtr->pinID);
				
				outPtr->links.clear();
				outPtr->connected = false;
				m_LinksDico.erase((uint32_t)outPtr->pinID);
				
				m_Links.erase(vLinkId);
			}
		}
	}
	else
	{
		LogVarDebug("Link id %i not found", vLinkId);
	}
}

void BaseNode::Break_VisualLinks_ConnectedToSlot(std::weak_ptr<NodeSlot> vSlot)
{
	if (!vSlot.expired())
	{
		auto slotPtr = vSlot.lock();
		if (slotPtr)
		{
			uint32_t pinId = (uint32_t)slotPtr->pinID;

			if (m_LinksDico.find(pinId) != m_LinksDico.end()) // trouve
			{
				auto linkIds = m_LinksDico[pinId]; // link Ptr pointe sur une entrée de m_Links
				for (auto lid : linkIds)
				{
					Del_VisualLink(lid);
				}
			}
		}
	}
}

void BaseNode::Break_VisualLink_ConnectedToSlots(std::weak_ptr<NodeSlot> vFrom, std::weak_ptr<NodeSlot> vTo)
{
	if (!vFrom.expired() &&
		!vTo.expired())
	{
		auto fromPtr = vFrom.lock();
		auto toPtr = vTo.lock();

		if (fromPtr && toPtr)
		{
			uint32_t fromPinId = (uint32_t)fromPtr->pinID;
			uint32_t endPinId = (uint32_t)toPtr->pinID;
			
			if (m_LinksDico.find(fromPinId) != m_LinksDico.end() &&
				m_LinksDico.find(endPinId) != m_LinksDico.end()) // trouve
			{
				// find link between vFrom and vTo
				uint32_t foundId = 0;
				auto fromLinks = m_LinksDico[fromPinId];
				auto toLinks = m_LinksDico[endPinId];
				for (auto fid : fromLinks)
				{
					for (auto tid : toLinks)
					{
						if (tid == fid)
						{
							foundId = tid;
							break;
						}
					}
					if (foundId)
						break;
				}

				// delete link visually
				if (foundId)
				{
					Del_VisualLink(foundId);
				}
			}

			// averti le parent que les slot on changé leur statut de connection
			fromPtr->NotifyConnectionChangeToParent(true);
			toPtr->NotifyConnectionChangeToParent(true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// CONNECT / DISCONNECT SLOTS BEHIND ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

bool BaseNode::ConnectSlots(std::weak_ptr<NodeSlot> vFrom, std::weak_ptr<NodeSlot> vTo)
{
	bool res = false;

	if (!vFrom.expired() &&
		!vTo.expired())
	{
		auto fromPtr = vFrom.lock();
		auto toPtr = vTo.lock();
		
		if (fromPtr && toPtr)
		{
			//bool swapped = false;

			// ensure that start is an output and end an input
			if (fromPtr->slotPlace == NodeSlot::NodeSlotPlaceEnum::NODE_SLOT_PLACE_INPUT &&
				toPtr->slotPlace == NodeSlot::NodeSlotPlaceEnum::NODE_SLOT_PLACE_OUTPUT)
			{
				//peu etre que std::swap merde quand c'est des shared_ptr ou weak_ptr
				vFrom.swap(vTo); 
				fromPtr = vTo.lock();
				toPtr = vFrom.lock();
			}

			if (!fromPtr->parentNode.expired() && 
				!toPtr->parentNode.expired())
			{
				auto fromParentNodePtr = fromPtr->parentNode.lock();
				auto toParentNodePtr = toPtr->parentNode.lock();
				if (fromParentNodePtr && toParentNodePtr)
				{
					if (toPtr->CanWeConnectToSlot(vFrom))
					{
						if (BlueprintManager::Instance()->ConnectNodeSlots(vFrom, vTo))
						{
							//LogVarDebug("Connection Success from %s to %s", fromParentNodePtr->name.c_str(), toParentNodePtr->name.c_str());

							// un output peut etre connecté a plusieurs inputs
							// un input ne peut etre connecté qu'a un seul output
							DestroySlotOfAnyMap(vTo);
							Add_VisualLink(vFrom, vTo);

							// averti le parent que les slot on changé leur statut de connection
							fromPtr->NotifyConnectionChangeToParent(true);
							toPtr->NotifyConnectionChangeToParent(true);

							res = true;
						}
					}
					
					if (!res)
					{
						std::string fromTypeStr = uType::ConvertUniformsTypeEnumToString(fromPtr->type);
						std::string toTypeStr = uType::ConvertUniformsTypeEnumToString(toPtr->type);

						if (!fromParentNodePtr->m_ParentNode.expired())
						{
							auto parentParentNodePtr = fromParentNodePtr->m_ParentNode.lock();
							if (parentParentNodePtr)
							{
								LogVarDebug("graph(%s) => Cant connect slots :\n\t- slot(stamp:%s,type:%s) from node(%s)\n\t- to\n\t- slot(stamp:%s,type:%s) from node(%s)\n",
									parentParentNodePtr->name.c_str(),
									fromPtr->stamp.typeStamp.c_str(), fromTypeStr.c_str(), fromParentNodePtr->name.c_str(),
									toPtr->stamp.typeStamp.c_str(), toTypeStr.c_str(), toParentNodePtr->name.c_str());
							}
						}
						else
						{
							//LogVarDebug("Was Swapped : %s", swapped ? "true" : "false");
							LogVarDebug("graph(NULL) => Cant connect slots :\n\t- slot(stamp:%s,type:%s) from node(%s)\n\t- to\n\t- slot(stamp:%s,type:%s) from node(%s)\n",
								fromPtr->stamp.typeStamp.c_str(), fromTypeStr.c_str(), fromParentNodePtr->name.c_str(),
								toPtr->stamp.typeStamp.c_str(), toTypeStr.c_str(), toParentNodePtr->name.c_str());
						}
					}
				}
				else
				{
					LogVarDebug("Le node parent du slot 'From' et 'to' est vide");
				}
			}
			else
			{
				LogVarDebug("Le node parent du slot 'From' est vide");
			}
		}
		else
		{
			LogVarDebug("l'un des from et to ne peut pas etre locked");
		}
	}
	else
	{
		LogVarDebug("l'un des nodes from et to est expired");
	}

	if (res)
	{
		/*if (NodeGraphSystem::Instance()->m_AutoCompilOnChanges)
		{
			NodeGraphSystem::Instance()->m_NeedReCompilation = true;
		}*/
	}

	return res;
}

bool BaseNode::DisConnectSlots(std::weak_ptr<NodeSlot> vFrom, std::weak_ptr<NodeSlot> vTo)
{
	bool res = false;

	/*if (RendererManager::Instance()->DisConnectRenderers(vFrom, vTo))
	{
		Break_VisualLink_ConnectedToSlots(vFrom, vTo);
		res = true;
	}
	else*/ if (BlueprintManager::Instance()->DisConnectNodeSlots(vFrom, vTo))
	{
		Break_VisualLink_ConnectedToSlots(vFrom, vTo);
		res = true;
	}
	return res;
}

bool BaseNode::DisConnectSlot(std::weak_ptr<NodeSlot> vSlot)
{
	bool res = true;

	auto slots = GetSlotsAssociatedToSlot(vSlot);
	for (auto slotPtr : slots)
	{
		if (/*RendererManager::Instance()->DisConnectRenderers(vSlot, slotPtr) ||*/
			BlueprintManager::Instance()->DisConnectNodeSlots(vSlot, slotPtr))
		{
			Break_VisualLink_ConnectedToSlots(vSlot, slotPtr);
		}
		else
		{
			res &= false;
		}
	}

	return res;
}

void BaseNode::NotifyConnectionChangeOfThisSlot(std::weak_ptr<NodeSlot> vSlot, bool vConnected) // ce solt a été connecté
{
	UNUSED(vConnected);
	if (!vSlot.expired())
	{
		auto ptr = vSlot.lock();
		if (ptr)
		{

		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// SLOT CONNECTION TEST ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

bool BaseNode::CanWeConnectSlots(std::weak_ptr<NodeSlot> vFrom, std::weak_ptr<NodeSlot> vTo)
{
	bool res = false;

	if (!vFrom.expired() &&
		!vTo.expired())
	{
		auto fromPtr = vFrom.lock();
		auto toPtr = vTo.lock();

		if (fromPtr && toPtr)
		{
			if (fromPtr && toPtr)
			{
				if (fromPtr->stamp.typeStamp == toPtr->stamp.typeStamp) res = true;

				// cest pas rationnel pour celui la, mais bon on fait confiance au graph
				// todo: idealemment quand on connect un vec3 a un type, le type devrait ce transformer en vec3
				// mais pas sur que le link des nodes ce fasse dans le bon ordre en fait, donc en attendant 
				// si le node from est type on dit que c'est ok
				if (fromPtr->stamp.typeStamp == "type") res = true;
				// c'est le meme pincipe pour le node to meme si ca doit plus souvent matcher
				// le truc c'est qu'on ne verifie pas le node from, on fait confiance au graph pour le moment
				if (toPtr->stamp.typeStamp == "type") res = true;

				// pareil pour vec pour le moment
				if (fromPtr->stamp.typeStamp == "vec") res = true;
				if (toPtr->stamp.typeStamp == "vec") res = true;

				// pareil pour mat pour le moment
				if (fromPtr->stamp.typeStamp == "mat") res = true;
				if (toPtr->stamp.typeStamp == "mat") res = true;

				if (fromPtr->stamp.typeStamp == "float")
				{
					if (toPtr->stamp.typeStamp == "vec") res = true;
					else if (toPtr->stamp.typeStamp == "vec2") res = true;
					else if (toPtr->stamp.typeStamp == "vec3") res = true;
					else if (toPtr->stamp.typeStamp == "vec4") res = true;
					else if (toPtr->stamp.typeStamp == "mat") res = true;
					else if (toPtr->stamp.typeStamp == "mat2") res = true;
					else if (toPtr->stamp.typeStamp == "mat3") res = true;
					else if (toPtr->stamp.typeStamp == "mat4") res = true;
				}

				if (toPtr->stamp.typeStamp == "vec")
				{
					if (fromPtr->stamp.typeStamp == "float") res = true;
					else if (fromPtr->stamp.typeStamp == "vec2") res = true;
					else if (fromPtr->stamp.typeStamp == "vec3") res = true;
					else if (fromPtr->stamp.typeStamp == "vec4") res = true;
				}
				
				// le swizzle est un node a aprt car meme si il renvoi un vec3
				// il peut avoir en input un vec2, vec3 ou vec4
				// genre vec4 f = vec2(0).xyyy;
				if (!res)
				{
					if (!toPtr->parentNode.expired())
					{
						auto parentPtr = toPtr->parentNode.lock();
						if (parentPtr)
						{
							/*if (parentPtr->m_NodeType == NodeTypeEnum::NODE_TYPE_SWIZZLE)
							{
								if (fromPtr->stamp.typeStamp == "vec" || fromPtr->stamp.typeStamp == "vec2" || fromPtr->stamp.typeStamp == "vec3" || fromPtr->stamp.typeStamp == "vec4")
								{
									if (fromPtr->stamp.typeStamp == "vec" || fromPtr->stamp.typeStamp == "vec2" || fromPtr->stamp.typeStamp == "vec3" || fromPtr->stamp.typeStamp == "vec4") res = true;
								}
							}*/
						}
					}
				}

				// flow
				if (!res)
				{
					if (!fromPtr->parentNode.expired() && !toPtr->parentNode.expired())
					{
						auto fp = fromPtr->parentNode.lock();
						auto tp = toPtr->parentNode.lock();
						if (fp && tp)
						{
							if (fp != tp) // pas droit de connecter un flow input a un flow output du meme node
							{
								if (fromPtr->type == uType::uTypeEnum::U_FLOW && toPtr->type == uType::uTypeEnum::U_FLOW)
								{
									res = true;
								}
							}
						}
					}
				}

				// Renderer Effects
				if (!res)
				{
					//assert(0);
					//res = UniformWidgetBase::CanTypesBeConnectedBoth(fromPtr->type, toPtr->type);
				}

				if (!res)
				{
					LogVarDebug("slot stamp %s seems not linkables with %s", fromPtr->stamp.typeStamp.c_str(), toPtr->stamp.typeStamp.c_str());
				}
			}
		}
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// SLOT SPLITTER ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::weak_ptr<NodeSlot>> BaseNode::InjectTypeInSlot(std::weak_ptr<NodeSlot> vSlotToSplit, uType::uTypeEnum vType)
{
	std::vector<std::weak_ptr<NodeSlot>> res;

	if (vType != uType::uTypeEnum::U_VOID)
	{
		if (!vSlotToSplit.expired())
		{
			auto slotPtr = vSlotToSplit.lock();
			if (slotPtr)
			{
				auto slotType = slotPtr->type;
				auto countChannels = uType::GetCountChannelForType(slotType);
				auto newCountChannels = uType::GetCountChannelForType(vType);
				if ((countChannels > 1U && newCountChannels < countChannels) || 
					slotPtr->IsGenericType())
				{

				}
			}
		}
	}
	else
	{
		LogVarDebug("Cant inject void type in slot");
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// CONFIGURATION ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

std::string BaseNode::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	std::string res;

	if (vUserDatas == "app")
	{

	}
	else if (vUserDatas == "project")
	{
		if (!m_ChildNodes.empty())
		{
			res += vOffset + "<graph>\n";

			// childs
			for (auto& node : m_ChildNodes)
			{
				res += vOffset + "\t<node name=\"" + node.second->name + "\" type=\"" +
					GetStringFromNodeTypeEnum(node.second->m_NodeType) + "\" pos=\"" +
					ct::fvec2(node.second->pos.x, node.second->pos.y).string() + "\" id=\"" +
					ct::toStr(node.second->nodeID) + "\">\n";
				res += node.second->getXml(vOffset + "\t", vUserDatas);
				res += vOffset + "\t</node>\n";
			}

			// links
			for (auto link : m_Links)
			{
				if (!link.second->in.expired() &&
					!link.second->out.expired())
				{
					auto inPtr = link.second->in.lock();
					auto outPtr = link.second->out.lock();
					if (inPtr && outPtr)
					{
						if (!inPtr->parentNode.expired() &&
							!outPtr->parentNode.expired())
						{
							auto inParentPtr = inPtr->parentNode.lock();
							auto outParentPtr = outPtr->parentNode.lock();

							if (inParentPtr && outParentPtr)
							{
								std::string inNodeIdSlotName = ct::toStr(inParentPtr->nodeID) + ":" + inPtr->name;
								std::string outNodeIdSlotName = ct::toStr(outParentPtr->nodeID) + ":" + outPtr->name;
								res += vOffset + "\t<link in=\"" + inNodeIdSlotName + "\" out=\"" + outNodeIdSlotName + "\"/>\n";
							}
						}
					}
				}
			}

			res += vOffset + "</graph>\n";
		}
		else
		{

		}
	}

	return res;
}

bool BaseNode::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
	// The value of this child identifies the name of this element
	std::string strName;
	std::string strValue;
	std::string strParentName;

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	if (vUserDatas == "app")
	{
		
	}
	else if (vUserDatas == "project")
	{
		if (strName == "node" && strParentName == "graph")
		{
			NodeTypeEnum _type = NodeTypeEnum::NODE_TYPE_NONE;
			ct::fvec2 _pos;
			size_t _nodeId = 0;
			std::string _name;

			for (const tinyxml2::XMLAttribute* attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
			{
				std::string attName = attr->Name();
				std::string attValue = attr->Value();

				if (attName == "type")
					_type = GetNodeTypeEnumFromString(attValue);
				if (attName == "pos")
					_pos = ct::fvariant(attValue).GetV2();
				if (attName == "id")
					_nodeId = ct::ivariant(attValue).GetU();
				if (attName == "name")
					_name = attValue;
			}

			return LoadNode(vElem, vParent, vUserDatas, _name, _type, _pos, _nodeId);
		}

		if (strName == "link" && strParentName == "graph")
		{
			std::string inStr;
			std::string outStr;

			for (const tinyxml2::XMLAttribute* attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
			{
				std::string attName = attr->Name();
				std::string attValue = attr->Value();

				if (attName == "in")
					inStr = attValue;
				if (attName == "out")
					outStr = attValue;
			}

			auto vecIn = ct::splitStringToVector(inStr, ':');
			auto vecOut = ct::splitStringToVector(outStr, ':');

			if (vecIn.size() == 2 && vecOut.size() == 2)
			{
				SlotEntry entIn;
				entIn.first = (uint32_t)ct::ivariant(vecIn[0]).GetU();
				entIn.second = vecIn[1];

				SlotEntry entOut;
				entOut.first = (uint32_t)ct::ivariant(vecOut[0]).GetU();
				entOut.second = vecOut[1];

				LinkEntry link;
				link.first = entIn;
				link.second = entOut;

				m_LinksToBuildAfterLoading.push_back(link);
			}
		}
	}	

	return true;
}

bool BaseNode::LoadNode(
	tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas,
	std::string vName, NodeTypeEnum vNodeType, ct::fvec2 vPos, size_t vNodeId)
{
	UNUSED(vElem);
	UNUSED(vParent);
	UNUSED(vUserDatas);
	UNUSED(vNodeType);
	UNUSED(vPos);
	bool continueXMLParsing = true;

	// pour eviter que des slots aient le meme id qu'un node
	freeNodeId = ct::maxi<uint32_t>(freeNodeId, (uint32_t)vNodeId);
	
	/*if (vNodeType == NodeTypeEnum::NODE_TYPE_COMPUTE_EFFECT)
	{
		auto node = ComputeNode::Create();
		if (node)
		{
			if (!vName.empty())
				node->name = vName;
			node->pos = ImVec2(vPos.x, vPos.y);
			node->nodeID = vNodeId;
			AddChildNode(node);
			node->RecursParsingConfig(vElem, vParent, vUserDatas);
			continueXMLParsing = false;
		}
	}
	else if (vNodeType == NodeTypeEnum::NODE_TYPE_FRAGMENT_EFFECT)
	{
		auto node = FragmentNode::Create();
		if (node)
		{
			if (!vName.empty())
				node->name = vName;
			node->pos = ImVec2(vPos.x, vPos.y);
			node->nodeID = vNodeId;
			AddChildNode(node);
			node->RecursParsingConfig(vElem, vParent, vUserDatas);
			continueXMLParsing = false;
		}
	}*/

	return continueXMLParsing;
}