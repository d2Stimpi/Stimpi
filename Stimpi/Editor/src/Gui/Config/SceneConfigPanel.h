#pragma once

namespace Stimpi
{
	class SceneConfigPanel
	{
	public:
		SceneConfigPanel();
		~SceneConfigPanel();

		void OnImGuiRender();

		bool ShowCheckerboardBg();

	private:
		bool m_Show = true;

		// Configurable data
		bool m_ShowCheckerboardBg = true;
		bool m_ShowColliderOutline = false;
		bool m_EnableSceneDebugMode = false;
		bool m_EnableCollisionContactPoints = false;
	};
}