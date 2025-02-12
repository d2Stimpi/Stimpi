#include "stpch.h"
#include "Gui/Utils/EditorResources.h"

#include "Stimpi/Asset/TextureImporter.h"

#define LOAD_ICON(IconName) m_Icons[IconName] = TextureImporter::LoadTexture(s_Config.m_ResourcePath / IconName)

namespace Stimpi
{
	EditorResourcesConfig s_Config;

	EditorIcons EditorResources::m_Icons;

	void EditorResources::LoadTextues()
	{
		LOAD_ICON(EDITOR_ICON_GEAR);
		LOAD_ICON(EDITOR_ICON_CUBE);
		LOAD_ICON(EDITOR_ICON_WCUBE);
		LOAD_ICON(EDITOR_ICON_SCRIPT);
		LOAD_ICON(EDITOR_ICON_RB2D);
		LOAD_ICON(EDITOR_ICON_ANIM);
		LOAD_ICON(EDITOR_ICON_CAM);
		LOAD_ICON(EDITOR_ICON_SPRITE);
		LOAD_ICON(EDITOR_ICON_CROSS);
		LOAD_ICON(EDITOR_ICON_SEARCH);
		LOAD_ICON(EDITOR_ICON_HAND);
		LOAD_ICON(EDITOR_ICON_MOVE);
		LOAD_ICON(EDITOR_ICON_SCALE);
	}

	Stimpi::Texture* EditorResources::GetIconTexture(std::string name)
	{
		auto it = m_Icons.find(name);
		if (it == m_Icons.end())
			return nullptr;

		return it->second.get();
	}

	ImTextureID EditorResources::GetIconTextureID(std::string name)
	{
		auto iconTexture = GetIconTexture(name);
		if (iconTexture != nullptr)
			return (void*)(intptr_t)iconTexture->GetTextureID();
		else
			return 0;
	}

}