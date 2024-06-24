#pragma once

namespace Stimpi
{
	class GraphicsConfigPanel
	{
	public:
		void OnImGuiRender();

		static void ShowWindow(bool show);
		static bool IsVisible();

	private:
		static bool m_Show;
	};
}