#pragma once

#include "Stimpi/Scene/Scene.h"

using namespace Stimpi;

namespace StimpiTest
{
	class Stimpi::Entity;

	enum class ComponentType 
	{ 
		ANIMATED_SPRITE,
		BOX_COLLIDER_2D,
		CAMERA,
		CIRCLE,
		NATIVE_SCRIPT,
		QUAD,
		RIGID_BODY_2D,
		SCRIPT,
		SORTING_GROUP,
		SPRITE
	};

	struct EntityComponentList
	{
		std::vector<ComponentType> m_ComponentList;

		EntityComponentList(std::initializer_list<ComponentType> list)
		{
			for (auto& type : list)
			{
				m_ComponentList.push_back(type);
			}
		}

		std::vector<ComponentType>::iterator begin() { return m_ComponentList.begin(); }
		std::vector<ComponentType>::iterator end() { return m_ComponentList.end(); }
	};

	class EntityFactory
	{
	public:
		static EntityFactory* Instance();
		static void SetContext(Scene* scene);

		static Entity CreateEntity(EntityComponentList list);

	private:
		Entity CreateEntityI(EntityComponentList list);

	private:
		Scene* m_Scene;
	};
}