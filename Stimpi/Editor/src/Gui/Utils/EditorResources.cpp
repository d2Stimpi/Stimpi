#include "stpch.h"
#include "Gui/Utils/EditorResources.h"

namespace Stimpi
{
	EditorResourcesConfig s_Config;

	EditorIcons s_EditorIcons;

	void EditorResources::LoadTextues()
	{
		s_EditorIcons.m_GearIcon = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / EDITOR_ICON_GEAR);
		s_EditorIcons.m_GearIcon = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / EDITOR_ICON_CUBE);
		s_EditorIcons.m_GearIcon = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / EDITOR_ICON_WCUBE);
		s_EditorIcons.m_GearIcon = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / EDITOR_ICON_SCRIPT);
		s_EditorIcons.m_GearIcon = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / EDITOR_ICON_RB2D);
		s_EditorIcons.m_GearIcon = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / EDITOR_ICON_ANIM);
		s_EditorIcons.m_GearIcon = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / EDITOR_ICON_CAM);
		s_EditorIcons.m_GearIcon = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / EDITOR_ICON_SPRITE);
	}

	Stimpi::Texture* EditorResources::GetIconTexture(std::string name)
	{
		AssetHandle iconAsset =  AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / name);
		return AssetManager::GetAsset(iconAsset).As<Texture>();
	}

	ImTextureID EditorResources::GetIconTextureID(std::string name)
	{
		AssetHandle iconAsset = AssetManager::GetAssetNoRefCount<Texture>(s_Config.m_ResourcePath / name);
		Texture* iconTexture = AssetManager::GetAsset(iconAsset).As<Texture>();
		if (iconTexture->Loaded())
			return (void*)(intptr_t)iconTexture->GetTextureID();
		else
			return 0;
	}

}