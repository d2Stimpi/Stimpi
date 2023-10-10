#include "stpch.h"
#include "Stimpi/Scene/SceneManager.h"

#include "Stimpi/Scene/SceneSerializer.h"

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
		m_ActiveScene.reset(new Scene());
		NotifyOnSceneChange();
	}

	void SceneManager::LoadScene(const std::string& filePath)
	{
		m_ActiveScene.reset(new Scene());
		SceneSerializer serializer(m_ActiveScene.get());
		serializer.Deseriealize(filePath);
		NotifyOnSceneChange();
	}

	void SceneManager::SaveScene(const std::string& filePath)
	{
		SceneSerializer serializer(m_ActiveScene.get());
		serializer.Serialize(filePath);
	}

	void SceneManager::NotifyOnSceneChange()
	{
		for (auto listener : m_OnSceneChangeListeners)
		{
			listener();
		}
	}

}