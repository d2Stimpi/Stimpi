#include "stpch.h"
#include "Stimpi/Scene/ResourceManager.h"

#include "Stimpi/Scene/Component.h"
#include "Stimpi/Log.h"

#include "Stimpi/Core/Project.h"
#include "Stimpi/Utils/YamlUtils.h"

#define DBG_LOG false

namespace Stimpi
{
	ResourceManager::ResourceManager()
	{

	}

	ResourceManager::~ResourceManager()
	{

	}

	ResourceManager* ResourceManager::Instance()
	{
		static auto m_Instance = std::make_unique<ResourceManager>();
		return m_Instance.get();
	}

	std::filesystem::path ResourceManager::GetProjectPathInternal()
	{
		return Project::GetProjectDir();
	}

	std::filesystem::path ResourceManager::GetAssetsPathInternal()
	{
		return Project::GetAssestsDir();
	}

	std::filesystem::path ResourceManager::GetScriptsPathInternal()
	{
		return Project::GetScriptsDir();
	}

	std::filesystem::path ResourceManager::GetResourcesPathInternal()
	{
		return Project::GetResourcesDir();
	}

	std::filesystem::path ResourceManager::GetDefaultProjectPathInternal()
	{
		return DEFAULT_PROJECT_PATH;
	}

	void ResourceManager::NotifyOnSceneChange()
	{
		for (auto& listener : m_OnProjectChangedListeners)
		{
			listener();
		}
	}


	bool ResourceManager::LoadDefaultProject()
	{
		// Load the default "Sandbox" project
		Project::Load("../Sandbox.d2sproj");

		// Verify selected/default project
		if (!Project::IsProjectPathValid())
		{
			ST_CORE_INFO("Invalid project configuration!");
			ST_CORE_ASSERT(true, "TODO - load fail handling");
		}

		return true;
	}

	void ResourceManager::ClearFile(const std::string& fileName)
	{
		std::ofstream outFile;
		outFile.open(fileName, std::ofstream::trunc);
		outFile.close();
	}

	void ResourceManager::WriteToFile(const std::string& fileName, const char* data)
	{
		std::ofstream outFile;
		outFile.open(fileName);
		outFile << data << std::endl;
		outFile.close();
	}

	void ResourceManager::WriteToFile(const std::string& fileName, const YAML::Node& data)
	{
		std::ofstream outFile;
		outFile.open(fileName);
		outFile << data << std::endl;
		outFile.close();
	}

	void ResourceManager::WriteToFileAppend(const std::string& fileName, const char* data)
	{
		std::ofstream outFile;
		outFile.open(fileName, std::ios_base::app);
		outFile << data << std::endl;
		outFile.close();
	}


	void ResourceManager::WriteToFileAppend(const std::string& fileName, const YAML::Node& data)
	{
		std::ofstream outFile;
		outFile.open(fileName, std::ios_base::app);
		outFile << data << std::endl;
		outFile.close();
	}

	bool ResourceManager::CompareYamlFileContent(const std::string& fileA, const std::string& fileB, const std::vector<std::string> ignoreKeys)
	{
		// TODO: add flag param to have more compare options: Raw, YAML, etc...

		if (fileA != fileB)
		{
			YAML::Node dataA = YAML::LoadFile(fileA);
			YAML::Node dataB = YAML::LoadFile(fileB);

			if (dataA.IsDefined() && dataB.IsDefined())
			{
				return YamlUtils::NodesEqual(dataA, dataB, ignoreKeys);
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	// TEMP test functions
	void ResourceManager::Test()
	{

	}
}