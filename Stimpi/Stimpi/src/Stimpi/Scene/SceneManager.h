#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Scene/Scene.h"

namespace Stimpi
{
	class ST_API SceneManager
	{
	public: 
		SceneManager();
		~SceneManager();

		static SceneManager* Instance();

		void SetActiveScene(Scene* scene) { m_ActiveScene = scene; }
		Scene* GetActiveScene() { return m_ActiveScene; }


	private:
		Scene* m_ActiveScene;

	};
}