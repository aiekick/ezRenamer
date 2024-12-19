#pragma once
#pragma warning(disable : 4100)

#include <set>
#include <array>
#include <string>
#include <memory>

#include <SoGLSL/Headers/SoGLSLDefs.h>

#include <ctools/cTools.h>
#include <ctools/ConfigAbstract.h>

#include <SoGLSL/Renderer/BaseRenderer.h>
#include <SoGLSL/ShaderPass/QuadShaderPass.h>

#include <Gaia/gaia.h>
#include <Gaia/Resources/Texture2D.h>
#include <Gaia/Core/VulkanCore.h>
#include <Gaia/Core/VulkanDevice.h>
#include <Gaia/Core/vk_mem_alloc.h>
#include <Gaia/Shader/VulkanShader.h>
#include <Gaia/Gui/ImGuiTexture.h>
#include <Gaia/Resources/VulkanRessource.h>
#include <Gaia/Resources/VulkanFrameBuffer.h>

#include <SoGLSL/Interfaces/GuiInterface.h>
#include <SoGLSL/Interfaces/NodeInterface.h>
#include <SoGLSL/Interfaces/TaskInterface.h>
#include <SoGLSL/Interfaces/NodeInterface.h>
#include <SoGLSL/Interfaces/ResizerInterface.h>

#include <SoGLSL/Interfaces/TextureInputInterface.h>
#include <SoGLSL/Interfaces/ShaderPassInputInterface.h>
#include <SoGLSL/Interfaces/TextureOutputInterface.h>

class SceneMergerModule_Quad_Pass;
class SceneMergerModule :
	public NodeInterface,
	public BaseRenderer,	
	public TaskInterface,
	public TextureInputInterface<0U>,
	public ShaderPassInputInterface,
	public TextureOutputInterface
{
public:
	static std::shared_ptr<SceneMergerModule> Create(GaiApi::VulkanCorePtr vVulkanCorePtr, BaseNodeWeak vParentNode);

private:
	std::weak_ptr<SceneMergerModule> m_This;
	FrameBufferPtr m_FrameBufferPtr = nullptr;
	SceneShaderPassContainer m_SceneShaderPassContainer;
	std::vector<uint32_t> m_JustDeletedSceneShaderPassSlots;

public:
	SceneMergerModule(GaiApi::VulkanCorePtr vVulkanCorePtr);
	~SceneMergerModule() override;

	bool Init();

	bool ExecuteAllTime(const uint32_t& vCurrentFrame, vk::CommandBuffer* vCmd = nullptr, BaseNodeState* vBaseNodeState = nullptr) override;
	bool ExecuteWhenNeeded(const uint32_t& vCurrentFrame, vk::CommandBuffer* vCmd = nullptr, BaseNodeState* vBaseNodeState = nullptr) override;
	void RenderShaderPasses(vk::CommandBuffer* vCmdBuffer) override;

	bool DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, const std::string& vUserDatas = {}) override;
	bool DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, const std::string& vUserDatas = {}) override;
	bool DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImVec2& vMaxSize, ImGuiContext* vContext = nullptr, const std::string& vUserDatas = {}) override;

	void NeedResizeByResizeEvent(ez::ivec2* vNewSize, const uint32_t* vCountColorBuffers) override;
	void NeedResizeByHand(ez::ivec2* vNewSize, const uint32_t* vCountColorBuffers) override;
	bool ResizeIfNeeded() override;

	// Interfaces Setters
	void SetTexture(const uint32_t& vBindingPoint, vk::DescriptorImageInfo* vImageInfo, ez::fvec2* vTextureSize = nullptr) override;
	void SetShaderPasses(const uint32_t& vSlotID, SceneShaderPassWeak vShaderPasses) override;

	// Interfaces Getters
	vk::DescriptorImageInfo* GetDescriptorImageInfo(const uint32_t& vBindingPoint, ez::fvec2* vOutSize = nullptr) override;

	SceneShaderPassContainer& GetSceneShaderPassContainerRef();
	const std::vector<uint32_t>& GetJustDeletedSceneShaderPassSlots() const;

	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
	void AfterNodeXmlLoading() override;
};
