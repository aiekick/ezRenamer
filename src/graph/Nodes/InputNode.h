#pragma once
#pragma warning(disable : 4100)

#include <SoGLSL/Graph/Graph.h>
#include <SoGLSL/Graph/Base/BaseNode.h>
#include <SoGLSL/Interfaces/TextureInputInterface.h>
#include <SoGLSL/Interfaces/ShaderPassInputInterface.h>
#include <SoGLSL/Interfaces/TextureOutputInterface.h>
#include <SoGLSL/Interfaces/ShaderUpdateInterface.h>

class SceneMergerModule;
class SceneMergerNode :
	public TextureInputInterface<0U>,
	public ShaderPassInputInterface,
	public TextureOutputInterface,
	public ShaderUpdateInterface,
	public BaseNode
{
public:
	static std::shared_ptr<SceneMergerNode> Create(GaiApi::VulkanCorePtr vVulkanCorePtr);

private:
	std::shared_ptr<SceneMergerModule> m_SceneMergerModulePtr = nullptr;

public:
	SceneMergerNode();
	~SceneMergerNode() override;

	// Init / Unit
	bool Init(GaiApi::VulkanCorePtr vVulkanCorePtr) override;

	// Execute Task
	bool ExecuteAllTime(const uint32_t & vCurrentFrame, vk::CommandBuffer * vCmd = nullptr, BaseNodeState * vBaseNodeState = nullptr) override;

	// Draw Widgets
    bool DrawWidgets(const uint32_t& vCurrentFrame,
        ImGuiContext* vContextPtr = nullptr,
        const std::string& vUserDatas = {}) override;
    bool DrawOverlays(const uint32_t& vCurrentFrame,
        const ImRect& vRect,
        ImGuiContext* vContextPtr = nullptr,
        const std::string& vUserDatas = {}) override;
    bool DrawDialogsAndPopups(const uint32_t& vCurrentFrame,
        const ImVec2& vMaxSize,
        ImGuiContext* vContextPtr = nullptr,
        const std::string& vUserDatas = {}) override;
    void DisplayInfosOnTopOfTheNode(BaseNodeState* vBaseNodeState) override;

	// Resize
	void NeedResizeByResizeEvent(ez::ivec2 * vNewSize, const uint32_t * vCountColorBuffers) override;

	// Interfaces Setters
	void SetTexture(const uint32_t& vBindingPoint, vk::DescriptorImageInfo* vImageInfo, ez::fvec2* vTextureSize = nullptr) override;
	void SetShaderPasses(const uint32_t& vSlotID, SceneShaderPassWeak vShaderPasses) override;

	// Interfaces Getters
	vk::DescriptorImageInfo* GetDescriptorImageInfo(const uint32_t& vBindingPoint, ez::fvec2* vOutSize = nullptr) override;

	// Configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

	NodeSlotWeak AddPreDefinedInput(const NodeSlot& vNodeSlot) override;
	void BeforeNodeXmlLoading() override;
	void AfterNodeXmlLoading() override;

	// Shader Update
	void UpdateShaders(const std::set<std::string>& vFiles) override;

	// INPUTS UPDATE
	void ReorganizeSlots(const std::vector<uint32_t>& vSlotsToErase);
};
