#pragma once

#include <Graph/uTypes/uTypes.h>
#include "NodeStamp.h"
#include <imgui/imgui.h>
#include <string>
#include <vector>
#include <memory>

struct BaseNodeStateStruct;
struct ImRect;
class UniformWidgetBase;
class BaseNode;
class NodeSlot
{
public:
	enum class NodeSlotTypeEnum : uint8_t
	{
		NODE_SLOT_TYPE_NONE = 0,
		NODE_SLOT_TYPE_DEFAULT,
		NODE_SLOT_TYPE_Count
	};
	enum class NodeSlotPlaceEnum : uint8_t
	{
		NODE_SLOT_PLACE_INPUT = 0,
		NODE_SLOT_PLACE_OUTPUT,
		NODE_SLOT_PLACE_Count
	};

public:
	static size_t GetNewSlotId();
	static std::string GetStringFromNodeSlotTypeEnum(const NodeSlotTypeEnum& vNodeSlotTypeEnum);
	static ImVec4 GetSlotColorAccordingToType(uType::uTypeEnum vType);
	
public:
	std::weak_ptr<NodeSlot> m_This;

public:
	NodeSlotTypeEnum slotType = NodeSlotTypeEnum::NODE_SLOT_TYPE_NONE;
	NodeSlotPlaceEnum slotPlace = NodeSlotPlaceEnum::NODE_SLOT_PLACE_INPUT;

public:
	uint32_t pinID = 0;
	std::string name;
	std::string help;
	NodeStamp stamp; // style vec3(vec3,float), pour le linking
	ImVec4 color = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);
	bool colorIsSet = false;
	uType::uTypeEnum type = uType::uTypeEnum::U_VOID;
	std::vector<std::weak_ptr<NodeSlot>> links;
	std::weak_ptr<BaseNode> parentNode;
	std::weak_ptr<UniformWidgetBase> uniform;
	uint32_t fboAttachmentId = 0;
	uint32_t channelId = 0; // by ex is this slot is the b channel of a vec4, channelId will be 2

	std::string originalFuncName;
	std::string funcCode; // code de la function si c'est un slot de type NODE_SLOT_TYPE_FUNCTION

	bool IsGenericType()
	{
		return 
			type == uType::uTypeEnum::U_TYPE || 
			type == uType::uTypeEnum::U_VEC || 
			type == uType::uTypeEnum::U_MAT;
	}
	bool IsGenericStampType()
	{
		return 
			stamp.typeStamp == "type" || 
			stamp.typeStamp == "vec" ||
			stamp.typeStamp == "mat";
	}

public:
	ImVec2 pos;
	bool highLighted = false;
	bool connected = false; // connect� a une autre node
	bool hideName = false; // n'affiche pas le nom du slot
	bool showWidget = false;
	bool hidden = false; // si true, il ne sera pas visible mais toujours present
	bool virtualUniform = false; // virtual si l'uniform n'est pas utilisé

public:
	explicit NodeSlot();
	explicit NodeSlot(std::string vName);
	explicit NodeSlot(std::string vName, uType::uTypeEnum vType);
	explicit NodeSlot(std::string vName, uType::uTypeEnum vType, bool vHideName);
	explicit NodeSlot(std::string vName, uType::uTypeEnum vType, bool vHideName, bool vShowWidget);
	~NodeSlot();

public:
	void Init();
	void Unit();

	std::string GetFullStamp();

	void NotifyConnectionChangeToParent(bool vConnected); // va contacter le parent pour lui dire que ce solt est connecté a un autre
	bool CanWeConnectToSlot(std::weak_ptr<NodeSlot> vSlot);

public: // splitter
	std::vector<std::weak_ptr<NodeSlot>> InjectTypeInSlot(uType::uTypeEnum vType);

public:
	void DrawContent(BaseNodeStateStruct *vCanvasState);
	void DrawSlot(BaseNodeStateStruct *vCanvasState, ImVec2 vSlotSize, ImVec2 vSlotOffset = ImVec2(0,0));
	
private:
	void DrawInputWidget(BaseNodeStateStruct *vCanvasState);
	void DrawOutputWidget(BaseNodeStateStruct *vCanvasState);
	void DrawSlotText(BaseNodeStateStruct *vCanvasState);

private:
	void DrawSlot(ImDrawList *vDrawList, ImVec2 vCenter, float vSlotRadius, bool vConnected, ImU32 vColor, ImU32 vInnerColor) const;
};