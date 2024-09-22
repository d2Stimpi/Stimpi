#pragma once

namespace Stimpi
{
	class LayersConfigPanel
	{
	public:
		LayersConfigPanel();

		void OnImGuiRender();
		void RemoveLayer(const size_t index);

		static void ShowWindow(bool show);
		static bool IsVisible();

	private:
		static bool m_Show;
	};
}