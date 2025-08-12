#pragma once

#include "Stimpi/Graphics/Texture.h"

#include "ImGui/src/imgui.h"

#include <filesystem>

#define EDITOR_ICON_GEAR		"gear.png"
#define EDITOR_ICON_CUBE		"cube.png"
#define EDITOR_ICON_WCUBE		"whitecube.png"
#define EDITOR_ICON_BCUBE		"bluecube.png"
#define EDITOR_ICON_SCRIPT		"script.png"
#define EDITOR_ICON_RB2D		"rb2d.png"
#define EDITOR_ICON_ANIM		"anim.png"
#define EDITOR_ICON_CAM			"cam.png"
#define EDITOR_ICON_SPRITE		"sprite.png"
#define EDITOR_ICON_CROSS		"cross.png"
#define EDITOR_ICON_SEARCH		"search.png"
#define EDITOR_ICON_HAND		"hand.png"
#define EDITOR_ICON_MOVE		"move.png"
#define EDITOR_ICON_SCALE		"scale.png"
#define EDITOR_ICON_NAV_BACK	"back.png"

// Node panel toolbar icons
#define EDITOR_ICON_COMPILE		"compile.png"
#define EDITOR_ICON_COMPILE_WRN "compile_wrn.png"
#define EDITOR_ICON_SAVE		"save.png"
#define EDITOR_ICON_SAVE_WRN	"save_wrn.png"

// Default style initializer values
constexpr ImVec2 s_DefaultIconButtonSize = { 22.0f, 21.0f };
constexpr ImVec2 s_DefaultSmallIconSize = { 16.0f, 16.0f };
constexpr ImVec2 s_DefaultMediumIconSize = { 32.0f, 32.0f };

// Colors
constexpr ImVec4 s_BlueTextColor = { 0.26f, 0.62f, 1.0f, 1.0f };

// Temp file paths
constexpr const char* s_TempPrefabPath = "temp.fab";

namespace Stimpi
{
	using EditorIcons = std::unordered_map<std::string, std::shared_ptr<Texture>>;

	struct EditorResourcesConfig
	{
		std::filesystem::path m_ResourcePath = "../resources/icons/";
	};

	class EditorResources
	{
	public:
		static void LoadTextues();

		static Texture* GetIconTexture(std::string name);
		static ImTextureID GetIconTextureID(std::string name);
	private:
		static EditorIcons m_Icons;
	};
}