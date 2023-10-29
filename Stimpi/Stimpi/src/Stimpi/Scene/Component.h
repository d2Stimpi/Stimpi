#pragma once

#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/ScriptableEntity.h"
#include "Stimpi/Scene/Camera.h"

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

		// Used for DragDropTarget
		void SetPayload(const std::string& filePath)
		{
			auto newTexture = ResourceManager::Instance()->LoadTexture(filePath);
			if (newTexture != nullptr)
			{
				m_FilePath = filePath;
				m_Texture = newTexture;
			}
		}

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

	// TODO: add zoom value in serialization
	struct CameraComponent
	{
		std::shared_ptr<Camera> m_Camera = nullptr;
		bool m_IsMain = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(std::shared_ptr<Camera> camera, bool isMain)
			: m_Camera(camera), m_IsMain(isMain) {}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Position" << YAML::Value;
				out << YAML::BeginSeq;
				{
					glm::vec3 position = m_Camera->GetOrthoCamera()->GetPosition();
					out << position.x << position.y << position.z;
				}
				out << YAML::EndSeq;

				out << YAML::Key << "Rotation" << YAML::Value << m_Camera->GetOrthoCamera()->GetRotation();

				out << YAML::Key << "ViewQuad" << YAML::Value;
				out << YAML::BeginSeq;
				{
					glm::vec4 view = m_Camera->GetOrthoCamera()->GetViewQuad();
					out << view.x << view.y << view.z << view.w;
				}
				out << YAML::EndSeq;

				out << YAML::Key << "IsMain" << YAML::Value << m_IsMain;
			}
			out << YAML::EndMap;
		}

		//De-serialize constructor
		CameraComponent(const YAML::Node& node)
		{
			if (node["ViewQuad"])
			{
				YAML::Node view = node["ViewQuad"];
				m_Camera = std::make_shared<Camera>(view[0].as<float>(), view[1].as<float>(), view[2].as<float>(), view[3].as<float>());
			}
			if (node["Position"])
			{
				YAML::Node positionNode = node["Position"];
				m_Camera->SetPosition(glm::vec3(positionNode[0].as<float>(), positionNode[1].as<float>(), positionNode[2].as<float>()));
			}
			if (node["Rotation"])
			{
				m_Camera->SetRotation(node["Rotation"].as<float>());
			}
			if (node["IsMain"])
			{
				m_IsMain = node["IsMain"].as<bool>();
			}
		}
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* m_Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->m_Instance; nsc->m_Instance = nullptr; };
		}
	};
}

	