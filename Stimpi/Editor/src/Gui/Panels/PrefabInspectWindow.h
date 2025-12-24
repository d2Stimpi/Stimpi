#pragma once

#include "Stimpi/Core/Timestep.h"
#include "Stimpi/Asset/Asset.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class Entity;
	class Scene;

	class PrefabInspectWindow
	{
	public:
		PrefabInspectWindow();
		~PrefabInspectWindow();

		void OnImGuiRender(Timestep ts);

		static void ShowWindow(bool show);
		static bool IsVisible();

		void SetPrefabEntity(AssetHandle prefabHandle);
		Entity GetPrefabEntity();
		Scene* GetScene();

	private:
		static bool m_Show;
	};
}