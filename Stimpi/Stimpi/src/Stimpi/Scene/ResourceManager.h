#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Texture.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class ST_API ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		static ResourceManager* Instance();

		void ClearFile(const std::string& fileName);
		void WriteToFile(const std::string& fileName, const char* data);
		void WriteToFile(const std::string& fileName, const YAML::Node& data);
		void WriteToFileAppend(const std::string& fileName, const char* data);
		void WriteToFileAppend(const std::string& fileName, const YAML::Node& data);

		// Texture resource managment
		Texture* LoadTexture(const std::string& fileName);
		void UnloadTextures();

		// TODO: tmp test
		void Test();

	private:
		std::map<std::string, std::shared_ptr<Texture>> m_TextureMap;
	};
}