#include "stpch.h"
#include "Stimpi/Core/ProjectSerializer.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{

	bool ProjectSerializer::Serialize(std::filesystem::path projectFilePath, ProjectConfig project)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Project" << YAML::Value;
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Name" << YAML::Value << project.m_Name;
				out << YAML::Key << "StartingScene" << YAML::Value << project.m_StartingScene;
				out << YAML::Key << "ProjectDir" << YAML::Value << project.m_ProjectDir.string();
				out << YAML::Key << "AssetsSubDir" << YAML::Value << project.m_AssestsSubDir.string();
				out << YAML::Key << "ScriptsSubDir" << YAML::Value << project.m_ScriptsSubDir.string();

				// Graphic config
				out << YAML::Key << "Graphics" << YAML::Value;
				out << YAML::BeginMap;
				{
					out << YAML::Key << "RenderingOrderAxis" << YAML::Value << RenderingOrderAxisToString(project.m_GraphicsConfig.m_RenderingOrderAxis);
				}
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(projectFilePath.string(), out.c_str());
		return true;
	}

	ProjectConfig ProjectSerializer::Deserialize(std::filesystem::path projectFilePath)
	{
		YAML::Node loadData = YAML::LoadFile(projectFilePath.string());
		YAML::Node node = loadData["Project"];

		ProjectConfig config;

		if (node["Name"])
		{
			config.m_Name = node["Name"].as<std::string>();
		}
		if (node["StartingScene"])
		{
			config.m_StartingScene = node["StartingScene"].as<std::string>();
		}
		if (node["ProjectDir"])
		{
			config.m_ProjectDir = node["ProjectDir"].as<std::string>();
		}
		if (node["AssetsSubDir"])
		{
			config.m_AssestsSubDir = node["AssetsSubDir"].as<std::string>();
		}
		if (node["ScriptsSubDir"])
		{
			config.m_ScriptsSubDir = node["ScriptsSubDir"].as<std::string>();
		}

		// Graphics config
		if (node["Graphics"])
		{
			YAML::Node graphics = node["Graphics"];
			if (graphics["RenderingOrderAxis"])
			{
				config.m_GraphicsConfig.m_RenderingOrderAxis = StringToRenderingOrderAxis(graphics["RenderingOrderAxis"].as<std::string>());
			}
			else
			{
				config.m_GraphicsConfig.m_RenderingOrderAxis = RenderingOrderAxis::None;
			}
		}

		return config;
	}

}