#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

/**
 *  Changes required when adding new Component
 *  - [Entity] Serialization
 *  - [Scene] Serialization and processing components in context of scene
 *  - [Component] Implement Serialize function
 *  - [ImGui - SceneConfigWindow] Extend inspect window widgets 
 */

namespace Stimpi
{
	struct TagComponent
	{
		std::string m_Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: m_Tag(tag) {}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
				out << YAML::Key << "Tag" << YAML::Value << m_Tag;
			out << YAML::EndMap;
		}

		//De-serialize done in Scene - Every entity will have TagComponent
	};

	// TODO: ATM not used/needed
	struct TransformComponent
	{
		glm::vec4 m_Transform;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec4& transform)
			: m_Transform(transform) {}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Transform" << YAML::Value;
			out << YAML::BeginSeq;
			out << m_Transform.x << m_Transform.y << m_Transform.z << m_Transform.w;
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}
	};

	struct QuadComponent
	{
		float m_X{ 0.0f };
		float m_Y{ 0.0f };
		float m_Width{ 0.0f };
		float m_Height{ 0.0f };

		QuadComponent() = default;
		QuadComponent(const QuadComponent&) = default;
		QuadComponent(const glm::vec4& quad)
			: m_X(quad.x), m_Y(quad.y), m_Width(quad.z), m_Height(quad.w) {}

		operator glm::vec4() const { return glm::vec4(m_X, m_Y, m_Width, m_Height); }

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "QuadComponent";
			out << YAML::BeginSeq;
				out << m_X << m_Y << m_Width << m_Height;
			out << YAML::EndSeq;
		}

		//De-serialize constructor
		QuadComponent(const YAML::Node& node)
		{
			m_X = node[0].as<float>();
			m_Y = node[1].as<float>();
			m_Width = node[2].as<float>();
			m_Height = node[3].as<float>();
		}
	};

	struct TextureComponent
	{
		std::string m_FilePath = "";
		Texture* m_Texture = nullptr;

		TextureComponent() = default;
		TextureComponent(const TextureComponent&) = default;
		TextureComponent(Texture* texture)
			: m_FilePath(""), m_Texture(texture) {}
		TextureComponent(const std::string& filePath)
			: m_FilePath(filePath)
		{
			m_Texture = ResourceManager::Instance()->LoadTexture(filePath);
		}

		operator Texture* () const { return m_Texture; }

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "TextureComponent";
			out << YAML::BeginMap;
					out << YAML::Key << "FilePath" << YAML::Value << m_FilePath;
			out << YAML::EndMap;
		}

		//De-serialize constructor
		TextureComponent(const YAML::Node& node)
		{
			if (node["FilePath"])
			{
				m_FilePath = node["FilePath"].as<std::string>();
				m_Texture = ResourceManager::Instance()->LoadTexture(m_FilePath);
			}
			else
			{
				m_FilePath = "";
				m_Texture = nullptr;
			}
		}
	};
}

	