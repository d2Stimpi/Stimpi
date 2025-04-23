#include "stpch.h"
#include "Stimpi/Asset/Prefab.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/Component.h"

namespace Stimpi
{

	Prefab::Prefab(Entity entity)
	{
		Initialize(entity);
	}

	Prefab::Prefab(YAML::Node data)
		: m_Data(data)
	{

	}

	void Prefab::Initialize(Entity entity)
	{
		YAML::Emitter out;
		entity.Serialize(out);
		m_Data = YAML::Load(out.c_str());
	}

	void Prefab::Save(const FilePath& filePath)
	{
		ResourceManager::Instance()->WriteToFile(filePath, m_Data);
	}

	void Prefab::BuildComponents(Entity entity)
	{
		std::string tag = "Prefab";
		if (m_Data["TagComponent"])
		{
			tag = m_Data["TagComponent"]["Tag"].as<std::string>();
		}
		entity.AddComponent<TagComponent>(tag);
		if (m_Data["QuadComponent"])
		{
			entity.AddComponent<QuadComponent>(QuadComponent(m_Data["QuadComponent"]));
		}
		if (m_Data["HierarchyComponent"])
		{
			entity.AddComponent<HierarchyComponent>(HierarchyComponent(m_Data["HierarchyComponent"]));
		}
		if (m_Data["CircleComponent"])
		{
			entity.AddComponent<CircleComponent>(CircleComponent(m_Data["CircleComponent"]));
		}
		if (m_Data["SpriteComponent"])
		{
			entity.AddComponent<SpriteComponent>(SpriteComponent(m_Data["SpriteComponent"]));
		}
		if (m_Data["SortingGroupComponent"])
		{
			entity.AddComponent<SortingGroupComponent>(SortingGroupComponent(m_Data["SortingGroupComponent"]));
		}
		if (m_Data["AnimatedSpriteComponent"])
		{
			entity.AddComponent<AnimatedSpriteComponent>(AnimatedSpriteComponent(m_Data["AnimatedSpriteComponent"]));
		}
		if (m_Data["ScriptComponent"])
		{
			ScriptComponent& scriptComponent = entity.AddComponent<ScriptComponent>(ScriptComponent(m_Data["ScriptComponent"]));
			scriptComponent.PopulateScriptInstanceData(m_Data["ScriptComponent"]);
		}
		if (m_Data["CameraComponent"])
		{
			entity.AddComponent<CameraComponent>(CameraComponent(m_Data["CameraComponent"]));
		}
		if (m_Data["RigidBody2DComponent"])
		{
			entity.AddComponent<RigidBody2DComponent>(RigidBody2DComponent(m_Data["RigidBody2DComponent"]));
		}
		if (m_Data["BoxCollider2DComponent"])
		{
			entity.AddComponent<BoxCollider2DComponent>(BoxCollider2DComponent(m_Data["BoxCollider2DComponent"]));
		}
	}

	std::shared_ptr<Prefab> Prefab::Create()
	{
		return std::make_shared<Prefab>();
	}

	std::shared_ptr<Prefab> Prefab::Create(Entity entity)
	{
		return std::make_shared<Prefab>(entity);
	}

	std::shared_ptr<Prefab> Prefab::Create(const FilePath& filePath)
	{
		YAML::Node data = YAML::LoadFile(filePath);
		if (data.IsDefined())
		{
			return std::make_shared<Prefab>(data);
		}

		return nullptr;
	}

}