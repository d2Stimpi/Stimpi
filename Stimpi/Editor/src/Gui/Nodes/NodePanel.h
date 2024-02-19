#pragma once

#include "Stimpi/Scene/Assets/AssetManager.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class NodePanel
	{
	public:
		NodePanel();
		~NodePanel();

		void OnImGuiRender();

		static void ShowWindow(bool show);
		static bool IsVisible();

	private:
		AssetHandle m_HeaderImage;
	};
}