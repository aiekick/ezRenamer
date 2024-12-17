// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "BlueprintManager.h"

#include <Gui/ImWidgets.h>
#include <Graph/Base/BaseNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// INIT / UNIT ///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BlueprintManager::Init()
{
	return true;
}	

void BlueprintManager::Unit()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// RENDER ////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BlueprintManager::Render()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// RESIZE ////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BlueprintManager::Resize()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// IMGUI /////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BlueprintManager::DrawImGui()
{
#ifdef _DEBUG
	/*if (ImGui::CollapsingHeader("CodeTree"))
	{
		//m_ComputeRenderer->DisplayCodeTree();
	}*/
#endif

	/*if (ImGui::CollapsingHeader("ComputeRenderer"))
	{
		//float aw = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x;
		//ImGui::Image((ImTextureID)&m_UBO.imageComputeRendererForImGui, ImVec2(aw, aw * m_UBO.imageComputeRendererForImGuiRatio));

		//m_ComputeRenderer->DrawWidgets();
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// SHADERS ///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BlueprintManager::UpdateShaders(const std::set<std::string>& vFiles) const
{
	std::set<std::string> res;

	for (auto & file : vFiles)
	{
		if (file.find(".vert") != std::string::npos ||
			file.find(".frag") != std::string::npos ||
			file.find(".tess") != std::string::npos ||
			file.find(".eval") != std::string::npos ||
			file.find(".glsl") != std::string::npos ||
			file.find(".geom") != std::string::npos ||
			file.find(".comp") != std::string::npos)
		{
			res.emplace(file);
		}
	}

	if (!res.empty())
	{
		
	}
}

void BlueprintManager::SetOrUpdateCode(std::string vCode)
{
	UNUSED(vCode);
}

void BlueprintManager::SelectNodeForPreview(std::weak_ptr<BaseNode> vNode)
{
	if (!vNode.expired())
	{
		auto nodePtr = vNode.lock();
		if (nodePtr)
		{
			if (nodePtr->m_NodeType != NodeTypeEnum::NODE_TYPE_FUNCTION)
			{
				/*auto eff = static_pointer_cast<SceneNode>(nodePtr);
				if (eff)
				{
					DebugPane::Instance()->SelectNode(vNode);
					//Preview2DPane::Instance()->SetOrUpdateRenderer(eff->GetRenderer());
					if (nodePtr->m_NodeType == NodeTypeEnum::NODE_TYPE_FUNCTION ||
						nodePtr->m_NodeType == NodeTypeEnum::NODE_TYPE_FUNCTION_CALL ||
						nodePtr->m_NodeType == NodeTypeEnum::NODE_TYPE_FUNCTION_EXTRACTED ||
						nodePtr->m_NodeType == NodeTypeEnum::NODE_TYPE_FUNCTION_COPIED_EXTRACTED)
					{
						CodePane::Instance()->SelectNode(vNode);
					}
					//TuningPane::Instance()->SetOrUpdateRenderer(eff->GetRenderer());
				}*/
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// SET CAMERA / MOUSE ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BlueprintManager::SetMousePos(ct::fvec2 vNormalizedPos, ct::fvec2 vBufferSize, bool vDownButton[5])
{
	UNUSED(vNormalizedPos);
	UNUSED(vBufferSize);
	UNUSED(vDownButton);
}

void BlueprintManager::SetScreenSize(ct::uvec2 vScreenSize)
{
	UNUSED(vScreenSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// SLOTS /////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BlueprintManager::ConnectNodeSlots(std::weak_ptr<NodeSlot> vStart, std::weak_ptr<NodeSlot> vEnd)
{
	bool res = false;

	if (!vStart.expired() &&
		!vEnd.expired())
	{
		auto startPtr = vStart.lock();
		auto endPtr = vEnd.lock();
		if (startPtr && endPtr)
		{
			if (!startPtr->parentNode.expired() &&
				!endPtr->parentNode.expired())
			{
				auto uniStart = startPtr->parentNode.lock();
				auto uniEnd = endPtr->parentNode.lock();
				if (uniStart && uniEnd)
				{
					uniEnd->SetCodeDirty(true);
				
					uniStart->JustConnectedBySlots(vStart, vEnd);
					uniEnd->JustConnectedBySlots(vStart, vEnd);

					res = true;
				}
				else
				{
					LogVarDebug("Error, le shared ptr de vStart ou vEnd ne peut pas etre recupéré");
				}
			}
			else
			{
				LogVarDebug("Error, vStart ou vEnd est expiré");
			}
		}
	}
	
	return res;
}

bool BlueprintManager::DisConnectNodeSlots(std::weak_ptr<NodeSlot> vStart, std::weak_ptr<NodeSlot> vEnd)
{
	bool res = false;

	if (!vStart.expired() &&
		!vEnd.expired())
	{
		auto startPtr = vStart.lock();
		auto endPtr = vEnd.lock();
		if (startPtr && endPtr)
		{
			if (!startPtr->uniform.expired() &&
				!endPtr->uniform.expired())
			{
				auto uniStart = startPtr->uniform.lock();
				auto uniEnd = endPtr->uniform.lock();
				if (uniStart && uniEnd)
				{
					std::weak_ptr<UniformWidgetBase> emptyUniform;
					//uniStart->SetUniformPilot(emptyUniform);
					//uniEnd->SetUniformPilot(emptyUniform);
					res = true;
				}
				else
				{
					LogVarDebug("Error, le shared_ptr de vStart ou vEnd ne peut pas etre recupéré");
				}
			}
			else
			{
				LogVarDebug("Error, vStart ou vEnd est expiré");
			}

			if (!startPtr->parentNode.expired() &&
				!endPtr->parentNode.expired())
			{
				auto uniStart = startPtr->parentNode.lock();
				auto uniEnd = endPtr->parentNode.lock();
				if (uniStart && uniEnd)
				{
					uniEnd->SetCodeDirty(true);

					uniStart->JustDisConnectedBySlots(vStart, vEnd);
					uniEnd->JustDisConnectedBySlots(vStart, vEnd);

					uniStart->SetChanged();
					uniEnd->SetChanged();

					res = true;
				}
				else
				{
					LogVarDebug("Error, le shared ptr de vStart ou vEnd ne peut pas etre recupéré");
				}
			}
			else
			{
				LogVarDebug("Error, vStart ou vEnd est expiré");
			}
		}
	}

	// en fait si ca merde on doit pouvoir supprimer le lien quand meme
	// le but de cette fonc est plus de reinit le pilotage
	res = true;

	return res;
}