#include "stpch.h"
#include "Stimpi/Scene/SceneManager.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Scene/SceneSerializer.h"
#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Asset/SceneAssetBuilder.h"

namespace Stimpi
{
	SceneManager::SceneManager()
		: m_ActiveScene({})
	{

	}

	SceneManager::~SceneManager()
	{

	}

	SceneManager* SceneManager::Instance()
	{
		static auto m_Instance = std::make_unique<SceneManager>();
		return m_Instance.get();
	}

	void SceneManager::NewScene()
	{
		m_ActiveScenePath = "";
		m_ActiveScene = std::make_shared<Scene>();

		NotifyOnSceneChange();
	}

	void SceneManager::LoadScene(const std::string& filePath)
	{
		if (filePath != "tmp.d2s")
			m_ActiveScenePath = filePath;

		m_ActiveScene = std::make_shared<Scene>();
		m_ActiveScene->SetName(FilePath(filePath).GetFileNameStem());
		NotifyOnScenePreload(m_ActiveScene);
		SceneSerializer serializer(m_ActiveScene.get());
		serializer.Deseriealize(filePath);
		NotifyOnSceneChange();
	}

	void SceneManager::SaveScene(const std::string& filePath)
	{
		if (filePath != "tmp.d2s")
			m_ActiveScenePath = filePath;

		SceneSerializer serializer(m_ActiveScene.get());
		serializer.Serialize(filePath);
	}

	void SceneManager::SaveScene(Scene* scene, const FilePath& filePath)
	{
		if (filePath.GetFileName() != "tmp.d2s")
			m_ActiveScenePath = filePath;

		SceneSerializer serializer(scene);
		serializer.Serialize(filePath);
	}

	void SceneManager::LoadStartingScene()
	{
		LoadScene(Project::GetStartingScenePath().string());
	}

	void SceneManager::NotifyOnSceneChange()
	{
		for (auto& listener : m_OnSceneChangeListeners)
		{
			listener();
		}
	}

	void SceneManager::NotifyOnScenePreload(std::shared_ptr<Scene> scene)
	{
		for (auto& listener : m_OnScenePreloadListeners)
		{
			listener(scene);
		}
	}

}