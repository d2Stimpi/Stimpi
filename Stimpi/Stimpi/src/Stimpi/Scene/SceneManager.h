#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Scene/Scene.h"

namespace Stimpi
{
	using OnSceneChangedListener = std::function<void(void)>;

	class ST_API SceneManager
	{
	public: 
		SceneManager();
		~SceneManager();

		static SceneManager* Instance();

		void SetActiveScene(Scene* scene) { m_ActiveScene.reset(scene); }
		Scene* GetActiveScene() { return m_ActiveScene.get(); }
		std::shared_ptr<Scene> GetActiveSceneRef() { return m_ActiveScene; }

		void NewScene();
		void LoadScene(const std::string& filePath);
		void SaveScene(const std::string& filePath);

		void LoadStartingScene();

		void RegisterOnSceneChangeListener(OnSceneChangedListener listener) { m_OnSceneChangeListeners.emplace_back(listener); }
	private:
		void NotifyOnSceneChange();
	private:
		std::shared_ptr<Scene> m_ActiveScene;
		std::vector<OnSceneChangedListener> m_OnSceneChangeListeners;
	};
}