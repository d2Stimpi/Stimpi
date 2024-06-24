#pragma once

namespace Stimpi
{
	class LayersConfigPanel
	{
	public:
		LayersConfigPanel();

		void OnImGuiRender();

		static void ShowWindow(bool show);
		static bool IsVisible();

	private:
		static bool m_Show;
	};
}