#pragma once

#include "Stimpi/Scene/Assets/AssetManager.h"

#include "ImGui/src/imgui.h"

#include <filesystem>

#define EDITOR_ICON_GEAR	"gear.png"
#define EDITOR_ICON_CUBE	"cube.png"

namespace Stimpi
{
	struct EditorResourcesConfig
	{
		std::filesystem::path m_ResourcePath = "../resources/icons/";
	};

	struct EditorIcons
	{
		AssetHandle m_GearIcon;
	};

	class EditorResources
	{
	public:
		static void LoadTextues();

		static Texture* GetIconTexture(std::string name);
		static ImTextureID GetIconTextureID(std::string name);
	};
}