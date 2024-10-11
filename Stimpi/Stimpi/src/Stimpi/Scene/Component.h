#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/Animation/AnimatedSprite.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/ScriptableEntity.h"
#include "Stimpi/Scene/Camera.h"
#include "Stimpi/Scene/Assets/AssetManager.h"
#include "Stimpi/Scripting/ScriptEngine.h"
#include "Stimpi/Scripting/ScriptSerializer.h"

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
	class ComponentObserver
	{
	public:
		static void InitComponentObservers(entt::registry& reg, Scene* scene);
		static void DeinitConstructObservers(entt::registry& reg);
	};

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
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Size = { 0.0f, 0.0f };
		float m_Rotation{ 0.0f };

		// Internal for picking
		bool m_PickEnabled = true;

		QuadComponent() = default;
		QuadComponent(const QuadComponent&) = default;
		QuadComponent(const glm::vec3& pos, const glm::vec2 size, float rotation = 0.0f)
			: m_Position(pos), m_Size(size), m_Rotation(rotation) {}

		glm::vec2 Center() { return glm::vec2(m_Position.x, m_Position.y); }

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "QuadComponent";
			out << YAML::BeginSeq;
				out << m_Position.x << m_Position.y << m_Position.z << m_Size.x << m_Size.y << m_Rotation;
			out << YAML::EndSeq;
		}

		//De-serialize constructor
		QuadComponent(const YAML::Node& node)
		{
			// Temp backward compatibility check
			if (node[5])
			{
				m_Position.x = node[0].as<float>();
				m_Position.y = node[1].as<float>();
				m_Position.z = node[2].as<float>();
				m_Size.x = node[3].as<float>();
				m_Size.y = node[4].as<float>();
				if (node[5])
					m_Rotation = node[5].as<float>();
			}
			else
			{
				m_Position.x = node[0].as<float>();
				m_Position.y = node[1].as<float>();
				m_Position.z = 0.0f;
				m_Size.x = node[2].as<float>();
				m_Size.y = node[3].as<float>();
				if (node[4])
					m_Rotation = node[4].as<float>();
			}
		}
	};

	struct CircleComponent
	{
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_Size = { 0.0f, 0.0f };
		float m_Rotation{ 0.0f };
		glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float m_Thickness = 1.0f;
		float m_Fade = 0.005;

		CircleComponent() = default;
		CircleComponent(const CircleComponent&) = default;
		CircleComponent(glm::vec3 pos, glm::vec2 size, float thickness = 1.0f, float fade = 0.005f)
			: m_Position(pos), m_Size(size), m_Thickness(thickness), m_Fade(fade)
		{
		}

		operator glm::vec4() const { return glm::vec4(m_Position.x, m_Position.y, m_Size.x, m_Size.y); }
		glm::vec2 Center() { return glm::vec2(m_Position.x, m_Position.y); }

		float MaxRadius() { return m_Size.x > m_Size.y ? m_Size.x : m_Size.y; }

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "CircleComponent";
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Position" << YAML::Value;
				out << YAML::BeginSeq;
				{
					out << m_Position.x << m_Position.y << m_Position.z;
				}
				out << YAML::EndSeq;

				out << YAML::Key << "Size" << YAML::Value;
				out << YAML::BeginSeq;
				{
					out << m_Size.x << m_Size.y;
				}
				out << YAML::EndSeq;

				out << YAML::Key << "Rotation" << YAML::Value << m_Rotation;

				out << YAML::Key << "Color" << YAML::Value;
				out << YAML::BeginSeq;
				{
					out << m_Color.r << m_Color.g << m_Color.b << m_Color.a;
				}
				out << YAML::EndSeq;

				out << YAML::Key << "Thickness" << YAML::Value << m_Thickness;
				out << YAML::Key << "Fade" << YAML::Value << m_Fade;
			}
			out << YAML::EndMap;
		}

		//De-serialize constructor
		CircleComponent(const YAML::Node& node)
		{
			if (node["Position"])
			{
				YAML::Node view = node["Position"];
				m_Position = glm::vec3(view[0].as<float>(), view[1].as<float>(), view[2].as<float>());
			}

			if (node["Size"])
			{
				YAML::Node view = node["Size"];
				m_Size = glm::vec2(view[0].as<float>(), view[1].as<float>());
			}

			if (node["Rotation"])
			{
				m_Rotation = node["Rotation"].as<float>();
			}

			if (node["Color"])
			{
				YAML::Node view = node["Color"];
				m_Color = glm::vec4(view[0].as<float>(), view[1].as<float>(), view[2].as<float>(), view[3].as<float>());
			}

			if (node["Thickness"])
			{
				m_Thickness = node["Thickness"].as<float>();
			}

			if (node["Fade"])
			{
				m_Fade = node["Fade"].as<float>();
			}
		}
	};

	struct SpriteComponent
	{
		// Sprite Texture
		FilePath m_FilePath = "";
		AssetHandle m_TextureHandle = {};
		bool m_Enable = false; // Will override color use

		// Sprite Color
		glm::vec4 m_Color = {1.0f, 1.0f, 1.0f, 1.0f };

		// Runtime only use (Scripting)
		// If true, no rendering of component
		bool m_Disabled = false;

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(glm::vec4 color)
			: m_Color(color) {}
		SpriteComponent(const std::string& filePath)
			: m_FilePath(filePath), m_Enable(true)
		{
			if (m_FilePath.Exists())
			{
				m_TextureHandle = AssetManager::GetAsset<Texture>(filePath);
			}
		}

		operator Texture* () const { return AssetManager::GetAssetData<Texture>(m_TextureHandle); }

		bool TextureLoaded()
		{
			bool loaded = false;

			if (m_TextureHandle.IsValid())
			{
				Texture* texture = AssetManager::GetAssetData<Texture>(m_TextureHandle);
				if (texture)
					loaded = texture->Loaded();
			}

			return loaded;
		}

		void SetTexture(const std::string& filePath)
		{
			SetPayload(filePath);
		}

		// Used for DragDropTarget
		void SetPayload(const std::string& filePath)
		{
			// Check if we are trying to load the same asset and skip if true
			if (m_TextureHandle.IsValid())
			{
				Asset textureAsset = AssetManager::GetAsset(m_TextureHandle);
				FilePath newPath = { filePath };
				// Check only asset name rather than the full path
				if (newPath.GetFileName() == textureAsset.GetName())
				{
					return;
				}
				else
				{
					AssetManager::Release(m_TextureHandle);
				}
			}
			// Load new asset
			m_TextureHandle = AssetManager::GetAsset<Texture>(filePath);

 			if (m_TextureHandle.IsValid())
 			{
				m_FilePath = filePath;
				m_Enable = true;
			}
		}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap;
				out << YAML::Key << "FilePath" << YAML::Value << m_FilePath.GetRelativePath(Project::GetAssestsDir()).string();

				out << YAML::Key << "Enabled" << YAML::Value << m_Enable;

				out << YAML::Key << "Color" << YAML::Value;
				out << YAML::BeginSeq;
				{
					out << m_Color.x << m_Color.y << m_Color.z << m_Color.w;
				}
				out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		//De-serialize constructor
		SpriteComponent(const YAML::Node& node)
		{
			if (node["FilePath"])
			{
				if (!node["FilePath"].as<std::string>().empty())
					m_FilePath = Project::GetAssestsDir() / node["FilePath"].as<std::string>();

				if (m_FilePath.Exists())
				{
					m_TextureHandle = AssetManager::GetAsset<Texture>(m_FilePath);
				}
			}
			else
			{
				m_FilePath = "";
			}

			if (node["Enable"])
			{
				m_Enable = node["Enable"].as<bool>();
			}
			else
			{
				//m_Enable = m_Texture != nullptr;
				m_Enable = m_TextureHandle.IsValid();
			}

			if (node["Color"])
			{
				YAML::Node view = node["Color"];
				m_Color = glm::vec4(view[0].as<float>(), view[1].as<float>(), view[2].as<float>(), view[3].as<float>());
			}
			else
			{
				m_Color = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
			}
		}
	};

	struct SortingGroupComponent
	{
		std::string m_SortingLayerName = "Default";	// Name will reference the SortingLayer, when Layer gets removed,
													// we simply fail to match the layerName with existing layers
		uint32_t m_OrderInLayer = 0;

		SortingGroupComponent() = default;
		SortingGroupComponent(const SortingGroupComponent&) = default;
		SortingGroupComponent(const std::string layerName, uint32_t order)
			: m_SortingLayerName(layerName), m_OrderInLayer(order)
		{}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "SortingGroupComponent";
			out << YAML::BeginMap;
			{
				out << YAML::Key << "SortingLayerName" << YAML::Value << m_SortingLayerName;
				out << YAML::Key << "OrderInLayer" << YAML::Value << m_OrderInLayer;
			}
			out << YAML::EndMap;
		}

		SortingGroupComponent(const YAML::Node& node)
		{
			m_SortingLayerName = "Default";
			m_OrderInLayer = 0;

			if (node["SortingLayerName"])
			{
				m_SortingLayerName = node["SortingLayerName"].as<std::string>();
			}
			if (node["OrderInLayer"])
			{
				m_OrderInLayer = node["OrderInLayer"].as<uint32_t>();
			}
		}
	};

	/**
	 * TODO: Clean up unused functions, and add some convenience methods 
	 *	for loading/adding animation from assets
	 */
	struct AnimatedSpriteComponent
	{
		std::shared_ptr<AnimatedSprite> m_AnimSprite;
		std::shared_ptr<Animation> m_DefaultAnimation;

		std::unordered_map<std::string, std::shared_ptr<Animation>> m_Animations;
		bool m_AutoPlay = false; // Auto play the default anim on start

		AnimatedSpriteComponent(const AnimatedSpriteComponent&) = default;
		AnimatedSpriteComponent()
		{
			m_AnimSprite = std::make_shared<AnimatedSprite>();
		}

		bool IsAnimationSet()
		{
			return m_AnimSprite != nullptr;
		}

		void SetAnimation(const std::string& name)
		{
			if (m_Animations.find(name) != m_Animations.end())
			{
				auto& animation = m_Animations.at(name);
				m_AnimSprite->SetAnimation(animation);
			}
			else
			{
				m_AnimSprite->SetAnimation(m_DefaultAnimation);
			}
		}

		std::string GetActiveAnimationName()
		{
			auto& anim = m_AnimSprite->GetAnimation();
			return anim->GetName();
		}

		void SetDefailtAnimation(const std::string& filePath)
		{
			auto newAnim = std::make_shared<Animation>();
			newAnim.reset(Animation::Create(filePath));

			m_DefaultAnimation = newAnim;
		}

		void AddAnimation(const std::string& filePath)
		{
			auto newAnim = std::make_shared<Animation>();
			newAnim.reset(Animation::Create(filePath));

			m_Animations[newAnim->GetName()] = newAnim;
		}

		void RemoveAnimation(const std::string& name)
		{
			m_Animations.erase(name);
		}

		void Start()
		{
			if (m_AnimSprite)
				m_AnimSprite->Start();
		}

		void Pause()
		{
			if (m_AnimSprite)
				m_AnimSprite->Pause();
		}

		void Stop()
		{
			if (m_AnimSprite)
				m_AnimSprite->Stop();
		}

		void SetLooping(bool looping)
		{
			if (m_AnimSprite)
				m_AnimSprite->SetLooping(looping);
		}

		bool GetLooping()
		{
			if (m_AnimSprite)
				return m_AnimSprite->GetLooping();

			return false;
		}

		bool IsPlaying()
		{
			if (m_AnimSprite)
				return m_AnimSprite->GetAnimationState() == AnimationState::PAUSED ||
					m_AnimSprite->GetAnimationState() == AnimationState::RUNNING;

			return false;
		}

		void SetPlaybackSpeed(float speed)
		{
			if (m_AnimSprite)
				m_AnimSprite->SetPlaybackSpeed(speed);
		}

		float GetPlaybackSpeed()
		{
			if (m_AnimSprite)
				m_AnimSprite->GetPlaybackSpeed();
		}

		operator SubTexture* () const { return m_AnimSprite->GetSubTexture(); }

		bool Loaded()
		{
			if (m_AnimSprite)
				return m_AnimSprite->Loaded();

			return false;
		}

		SubTexture* GetSubTexture()
		{
			if (m_AnimSprite)
				return m_AnimSprite->GetSubTexture();

			return nullptr;
		}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "AnimatedSpriteComponent";
			out << YAML::BeginMap;
			{
				if (m_DefaultAnimation)
				{
					out << YAML::Key << "DefaultAnimation" << YAML::Value << m_DefaultAnimation->GetAssetFilePath().GetRelativePath(Project::GetAssestsDir()).string();
				}
				out << YAML::Key << "Animations";
				out << YAML::BeginMap;
				{
					for (auto& anim : m_Animations)
					{
						out << YAML::Key << "Animation";
						out << YAML::BeginMap;
						{
							out << YAML::Key << "AnimationAssetPath" << YAML::Value << anim.second->GetAssetFilePath().GetRelativePath(Project::GetAssestsDir()).string();
						}
						out << YAML::EndMap;
					}
				}
				out << YAML::EndMap;
				out << YAML::Key << "AutoPlay" << YAML::Value << m_AutoPlay;
			}
			out << YAML::EndMap;
		}

		AnimatedSpriteComponent(const YAML::Node& node)
		{
			m_AnimSprite = std::make_shared<AnimatedSprite>();
			m_AutoPlay = false;

			if (node["DefaultAnimation"])
			{
				FilePath defaultAnimPath = Project::GetAssestsDir() / node["DefaultAnimation"].as<std::string>();
				SetDefailtAnimation(defaultAnimPath);
				m_AnimSprite->SetAnimation(m_DefaultAnimation);
			}
			if (node["Animations"])
			{
				YAML::Node anims = node["Animations"];
				for (YAML::const_iterator it = anims.begin(); it != anims.end(); it++)
				{
					YAML::Node anim = it->second;
					if (anim["AnimationAssetPath"])
					{
						FilePath animPath = Project::GetAssestsDir() / anim["AnimationAssetPath"].as<std::string>();
						AddAnimation(animPath);
					}
				}
			}
			if (node["AutoPlay"])
			{
				m_AutoPlay = node["AutoPlay"].as<bool>();
			}
		}
	};

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

				out << YAML::Key << "Zoom" << YAML::Value << m_Camera->GetZoomFactor();

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
			if (node["Rotation"])
			{
				m_Camera->SetZoomFactor(node["Zoom"].as<float>());
			}
			if (node["IsMain"])
			{
				m_IsMain = node["IsMain"].as<bool>();
			}
		}
	};

	// Scripting

	struct ScriptComponent
	{
		std::string m_ScriptName;
		std::shared_ptr<ScriptInstance> m_ScriptInstance;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
		ScriptComponent(const std::string& scriptName)
			: m_ScriptName(scriptName)
		{
		}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			{
				out << YAML::Key << "ScriptName" << YAML::Value << m_ScriptName;
				if (m_ScriptInstance)
				{
					out << YAML::Key << "ScriptFields";
					out << YAML::BeginMap;
					{
						auto& fields = m_ScriptInstance->GetFields();
						auto& object = m_ScriptInstance->GetInstance();
						for (auto& item : fields)
						{
							auto& field = item.second;
							ScriptSeriaizer::SerializeScriptField(out, object.get(), field.get());
						}
					}
					out << YAML::EndMap;
				}
			}
			out << YAML::EndMap;
		}

		//De-serialize constructor
		ScriptComponent(const YAML::Node& node)
		{
			if (node["ScriptName"])
			{
				m_ScriptName = node["ScriptName"].as<std::string>();
			}
		}

		void PopulateScriptInstanceData(const YAML::Node& node)
		{
			if (node["ScriptFields"])
			{
				YAML::Node fields = node["ScriptFields"];
				for (YAML::const_iterator it = fields.begin(); it != fields.end(); it++)
				{
					YAML::Node fieldNode = it->second;
					auto& object = m_ScriptInstance->GetInstance();
					
					if (!fieldNode["Name"] || !fieldNode["Type"] || !fieldNode["FieldData"])
						return;

					std::string fieldName = fieldNode["Name"].as<std::string>();
					FieldType fieldType = ScriptSeriaizer::StringToFieldType(fieldNode["Type"].as<std::string>());
					if ((fieldType == FieldType::FIELD_TYPE_CLASS) || (fieldType == FieldType::FIELD_TYPE_STRUCT))
					{
						auto field = m_ScriptInstance->GetFieldByName(fieldName);
						if (field)
						{
							ScriptSeriaizer::DeserializeScriptField(fieldNode, object.get(), field.get());
						}
					}
					else
					{
						// TODO: handle other types
					}
				}
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

	// Physics

	struct RigidBody2DComponent
	{
		enum class BodyType { STATIC = 0, DYNAMIC, KINEMATIC };

		BodyType m_Type = BodyType::STATIC;
		bool m_FixedRotation = false;

		// For runtime
		void* m_RuntimeBody = nullptr;
		bool m_ShouldUpdateTransform = false;
		glm::vec2 m_DeferredTransformPos = { 0.0f, 0.0f };
		float m_DeferredTransfomrAngle = 0.0f;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;

		std::string RigidBody2DTypeToString(BodyType type)
		{
			switch (type)
			{
			case BodyType::STATIC:	return "Static";
			case BodyType::DYNAMIC:	return "Dynamic";
			case BodyType::KINEMATIC: return "Kinematic";
			}

			ST_CORE_ASSERT_MSG(false, "Unknow Rigid Body type");
			return "Unknown";
		}

		BodyType StringToRigidBody2DType(std::string typeString)
		{
			BodyType ret = BodyType::STATIC;

			typeString == "Static" ?	ret = BodyType::STATIC :
			typeString == "Dynamic" ?	ret = BodyType::DYNAMIC :
			typeString == "Kinematic" ? ret = BodyType::KINEMATIC :
			ret = BodyType::STATIC;

			return ret;
		}

		void SetTransformDeferred(glm::vec2 position, float angle)
		{
			m_ShouldUpdateTransform = true;
			m_DeferredTransformPos = position;
			m_DeferredTransfomrAngle = angle;
		}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Type" << YAML::Value << RigidBody2DTypeToString(m_Type);

				out << YAML::Key << "FixedRotation" << YAML::Value << m_FixedRotation;
			}
			out << YAML::EndMap;
		}

		//De-serialize constructor
		RigidBody2DComponent(const YAML::Node& node)
		{
			if (node["Type"])
			{
				m_Type = StringToRigidBody2DType(node["Type"].as<std::string>());
			}
			if (node["FixedRotation"])
			{
				m_FixedRotation = node["FixedRotation"].as<bool>();
			}
		}
	};

	struct BoxCollider2DComponent
	{
		enum class Collider2DShape { BOX = 0, CIRLCE };

		Collider2DShape m_ColliderShape = Collider2DShape::BOX;
		glm::vec2 m_Offset = { 0.0f, 0.0f };
		glm::vec2 m_Size = { 0.5f, 0.5f };	// Represents the 1m unit and Renderer's quad size

		float m_Density = 1.0f;
		float m_Friction = 0.5f;
		float m_Restitution = 0.0f;
		float m_RestitutionThreshold = 0.5f;

		// For runtime
		void* m_RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Shape" << YAML::Value << (uint32_t)m_ColliderShape;

				out << YAML::Key << "Offset" << YAML::Value;
				out << YAML::BeginSeq;
				{
					out << m_Offset.x << m_Offset.y;
				}
				out << YAML::EndSeq;

				out << YAML::Key << "Size" << YAML::Value;
				out << YAML::BeginSeq;
				{
					out << m_Size.x << m_Size.y;
				}
				out << YAML::EndSeq;

				out << YAML::Key << "Density" << YAML::Value << m_Density;

				out << YAML::Key << "Friction" << YAML::Value << m_Friction;

				out << YAML::Key << "Restitution" << YAML::Value << m_Restitution;

				out << YAML::Key << "RestitutionThreshold" << YAML::Value << m_RestitutionThreshold;
			}
			out << YAML::EndMap;
		}

		//De-serialize constructor
		BoxCollider2DComponent(const YAML::Node& node)
		{
			if (node["Shape"])
			{
				m_ColliderShape = (Collider2DShape)node["Shape"].as<uint32_t>();
			}

			if (node["Offset"])
			{
				YAML::Node offset = node["Offset"];
				m_Offset = glm::vec2(offset[0].as<float>(), offset[1].as<float>());
			}
			if (node["Size"])
			{
				YAML::Node size = node["Size"];
				m_Size = glm::vec2(size[0].as<float>(), size[1].as<float>());
			}
			if (node["Density"])
			{
				m_Density = node["Density"].as<float>();
			}
			if (node["Friction"])
			{
				m_Friction = node["Friction"].as<float>();
			}
			if (node["Restitution"])
			{
				m_Restitution = node["Restitution"].as<float>();
			}
			if (node["RestitutionThreshold"])
			{
				m_RestitutionThreshold = node["RestitutionThreshold"].as<float>();
			}
		}
	};
}

	