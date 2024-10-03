#pragma once

#include "Stimpi/Scene/Assets/AssetManager.h"

#include "ImGui/src/imgui.h"

#include <filesystem>

#define EDITOR_ICON_GEAR	"gear.png"
#define EDITOR_ICON_CUBE	"cube.png"
#define EDITOR_ICON_WCUBE	"whitecube.png"
#define EDITOR_ICON_SCRIPT	"script.png"
#define EDITOR_ICON_RB2D	"rb2d.png"
#define EDITOR_ICON_ANIM	"anim.png"
#define EDITOR_ICON_CAM		"cam.png"
#define EDITOR_ICON_SPRITE	"sprite.png"
#define EDITOR_ICON_CROSS	"cross.png"
#define EDITOR_ICON_SEARCH	"search.png"
#define EDITOR_ICON_HAND	"hand.png"
#define EDITOR_ICON_MOVE	"move.png"
#define EDITOR_ICON_SCALE	"scale.png"


// Default style initializer values
constexpr ImVec2 s_DefaultIconButtonSize = { 22.0f, 21.0f };
constexpr ImVec2 s_DefaultSmallIconSize = { 16.0f, 16.0f };

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