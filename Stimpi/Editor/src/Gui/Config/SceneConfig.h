#pragma once

namespace Stimpi
{
	class SceneConfig
	{
	public:
		SceneConfig();
		~SceneConfig();

		void OnImGuiRender();

		bool ShowCheckerboardBg();

	private:
		bool m_Show = true;

		// Configurable data
		bool m_ShowCheckerboardBg = true;
		bool m_ShowColliderOutline = false;
	};
}