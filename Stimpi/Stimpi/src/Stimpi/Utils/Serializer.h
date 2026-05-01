#pragma once

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class Entity;

	class DataBuilder
	{
	public:
		DataBuilder* BeginMap(const std::string& name);
		DataBuilder* EndMap();

		template<typename Component>
		DataBuilder* AddComponent(Entity entity)
		{
			if (entity.HasComponent<Component>())
				entity.GetComponent<Component>().Serialize(m_Emitter);

			return this;
		}

	private:
		YAML::Emitter m_Emitter{};
	};

	class Serializer
	{
	public:

		static DataBuilder* CreateDataBuilder();

	private:
	};
}