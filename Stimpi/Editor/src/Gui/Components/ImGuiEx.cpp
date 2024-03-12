#include "stpch.h"
#include "Gui/Components/ImGuiEx.h"

namespace Stimpi
{
	ImGuiExStyle s_Style;

	bool ImGuiEx::IconButton(const char* strID, std::string iconName)
	{
		bool retVal = false;
		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture->Loaded())
		{
			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			retVal = ImGui::ImageButton(strID, (void*)(intptr_t)iconTexture->GetTextureID(), s_Style.m_IconButtonSize, uv_min, uv_max);
		}

		return retVal;
	}

	void ImGuiEx::Icon(std::string iconName)
	{
		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture->Loaded())
		{
			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)iconTexture->GetTextureID(), s_Style.m_IconButtonSize, uv_min, uv_max);
		}
	}

	Stimpi::ImGuiExStyle& ImGuiEx::GetStyle()
	{
		return s_Style;
	}

}