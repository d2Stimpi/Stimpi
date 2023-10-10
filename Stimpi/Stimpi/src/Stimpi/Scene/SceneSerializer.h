#pragma once

#include "Stimpi/Scene/Scene.h"

namespace Stimpi
{
	class SceneSerializer
	{
	public:
		SceneSerializer(Scene* scene);

		void Serialize(const std::string& filePath);
		bool Deseriealize(const std::string& filePath);

	private:
		Scene* m_Scene;
	};
}