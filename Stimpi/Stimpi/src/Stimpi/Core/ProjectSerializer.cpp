#include "stpch.h"
#include "Stimpi/Core/ProjectSerializer.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{

	bool ProjectSerializer::Serialize(std::filesystem::path projectFilePath, ProjectConfig projectConfig)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Project" << YAML::Value;
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Name" << YAML::Value << projectConfig.m_Name;
				out << YAML::Key << "StartingScene" << YAML::Value << projectConfig.m_StartingScene;
				out << YAML::Key << "ProjectDir" << YAML::Value << projectConfig.m_ProjectDir.string();
				out << YAML::Key << "AssetsSubDir" << YAML::Value << projectConfig.m_AssestsSubDir.string();
				out << YAML::Key << "ScriptsSubDir" << YAML::Value << projectConfig.m_ScriptsSubDir.string();

				// Graphic config
				out << YAML::Key << "Graphics" << YAML::Value;
				out << YAML::BeginMap;
				{
					out << YAML::Key << "RenderingOrderAxis" << YAML::Value << RenderingOrderAxisToString(projectConfig.m_GraphicsConfig.m_RenderingOrderAxis);
				}
				out << YAML::EndMap;

				// Physics config
				out << YAML::Key << "Physics" << YAML::Value;
				out << YAML::BeginMap;
				{
					out << YAML::Key << "GravityFoce" << YAML::Value;
					out << YAML::BeginSeq;
					{
						out << projectConfig.m_PhysicsConfig.m_GravityForce.x << projectConfig.m_PhysicsConfig.m_GravityForce.y;
					}
					out << YAML::EndSeq;
				}
				out << YAML::EndMap;

				// SortingLayers config
				out << YAML::Key << "SortingLayers" << YAML::Value;
				out << YAML::BeginMap;
				{
					for (auto& layer : projectConfig.m_SortingLayers)
					{
						out << YAML::Key << "Layer" << YAML::Value;
						out << YAML::BeginMap;
						{
							out << YAML::Key << "Name" << YAML::Value << layer->m_Name;
						}
						out << YAML::EndMap;
					}
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

		ProjectConfig projectConfig;

		if (node["Name"])
		{
			projectConfig.m_Name = node["Name"].as<std::string>();
		}
		if (node["StartingScene"])
		{
			projectConfig.m_StartingScene = node["StartingScene"].as<std::string>();
		}
		if (node["ProjectDir"])
		{
			projectConfig.m_ProjectDir = node["ProjectDir"].as<std::string>();
		}
		if (node["AssetsSubDir"])
		{
			projectConfig.m_AssestsSubDir = node["AssetsSubDir"].as<std::string>();
		}
		if (node["ScriptsSubDir"])
		{
			projectConfig.m_ScriptsSubDir = node["ScriptsSubDir"].as<std::string>();
		}

		// Graphics config
		if (node["Graphics"])
		{
			YAML::Node graphics = node["Graphics"];
			if (graphics["RenderingOrderAxis"])
			{
				projectConfig.m_GraphicsConfig.m_RenderingOrderAxis = StringToRenderingOrderAxis(graphics["RenderingOrderAxis"].as<std::string>());
			}
			else
			{
				projectConfig.m_GraphicsConfig.m_RenderingOrderAxis = RenderingOrderAxis::None;
			}
		}

		// Physics config
		if (node["Physics"])
		{
			YAML::Node physics = node["Physics"];
			if (physics["GravityFoce"])
			{
				YAML::Node gravity = physics["GravityFoce"];
				projectConfig.m_PhysicsConfig.m_GravityForce = glm::vec2(gravity[0].as<float>(), gravity[1].as<float>());
			}
		}

		// SortingLayers config
		if (node["SortingLayers"])
		{
			YAML::Node sortingLayers = node["SortingLayers"];
			for (YAML::const_iterator it = sortingLayers.begin(); it != sortingLayers.end(); it++)
			{
				YAML::Node layerNode = it->second;
				std::shared_ptr<SortingLayer> newLayer;
				
				if (layerNode["Name"])
				{
					newLayer = std::make_shared<SortingLayer>(layerNode["Name"].as<std::string>());
				}

				if (newLayer)
					projectConfig.m_SortingLayers.emplace_back(newLayer);
			}
		}

		return projectConfig;
	}

}