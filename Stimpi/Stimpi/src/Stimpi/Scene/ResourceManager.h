#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Texture.h"

#include <yaml-cpp/yaml.h>
#include <filesystem>

#define DEFAULT_PROJECT_PATH "../assets";

namespace Stimpi
{
	using OnProjectChangedListener = std::function<void(void)>;

	class ST_API ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		static ResourceManager* Instance();

		// Project management
		static std::filesystem::path GetProjectPath() { return Instance()->GetProjectPathInternal(); }
		static std::filesystem::path GetAssetsPath() { return Instance()->GetAssetsPathInternal(); }
		static std::filesystem::path GetScriptsPath() { return Instance()->GetScriptsPathInternal(); }
		static std::filesystem::path GetResourcesPath() { return Instance()->GetResourcesPathInternal(); }
		static std::filesystem::path GetDefaultProjectPath() { return Instance()->GetDefaultProjectPathInternal(); }

		static bool LoadDefaultProject();

		void ClearFile(const std::string& fileName);
		void WriteToFile(const std::string& fileName, const char* data);
		void WriteToFile(const std::string& fileName, const YAML::Node& data);
		void WriteToFileAppend(const std::string& fileName, const char* data);
		void WriteToFileAppend(const std::string& fileName, const YAML::Node& data);

		// Texture resource management
		Texture* LoadTexture(const std::string& fileName);
		void UnloadTextures();

		// TODO: tmp test
		void Test();

		void RegisterOnProjectChangedListener(OnProjectChangedListener listener) { m_OnProjectChangedListeners.emplace_back(listener); }
	private:
		std::filesystem::path GetProjectPathInternal();
		std::filesystem::path GetAssetsPathInternal();
		std::filesystem::path GetScriptsPathInternal();
		std::filesystem::path GetResourcesPathInternal();
		std::filesystem::path GetDefaultProjectPathInternal();
		
		void NotifyOnSceneChange();

	private:
		std::map<std::string, std::shared_ptr<Texture>> m_TextureMap;

		// Project management
		std::filesystem::path m_ProjectDir = DEFAULT_PROJECT_PATH;
		std::vector<OnProjectChangedListener> m_OnProjectChangedListeners;
	};
}