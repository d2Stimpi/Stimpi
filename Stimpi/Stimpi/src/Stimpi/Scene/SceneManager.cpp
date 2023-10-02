#include "Stimpi/Scene/SceneManager.h"

namespace Stimpi
{
	SceneManager::SceneManager()
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

}