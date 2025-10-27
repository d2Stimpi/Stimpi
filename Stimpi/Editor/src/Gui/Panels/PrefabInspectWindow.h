#pragma once

#include "Stimpi/Core/Timestep.h"
#include "Stimpi/Asset/Asset.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class PrefabInspectWindow
	{
	public:
		PrefabInspectWindow();
		~PrefabInspectWindow();

		void OnImGuiRender(Timestep ts);

		static void ShowWindow(bool show);
		static bool IsVisible();

		void SetPrefabEntity(AssetHandle prefabHandle);

	private:
		static bool m_Show;
	};
}