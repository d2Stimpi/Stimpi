#include "Stimpi/Scene/ResourceManager.h"

#include "Stimpi/Scene/Component.h"

namespace Stimpi
{
	ResourceManager* ResourceManager::Instance()
	{
		static auto m_Instance = std::make_unique<ResourceManager>();
		return m_Instance.get();
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

	void ResourceManager::Test()
	{
		YAML::Node node;
		auto tag = TagComponent{ "Camera" };
		auto transorm = TransformComponent{ glm::vec4(1.5f, 1.2f, 1.1f, 1.0f) };

		node["TagComponent"] = tag;
		node["TransformComponent"] = transorm;
		WriteToFileAppend("TestFile.data", node);
	}
}