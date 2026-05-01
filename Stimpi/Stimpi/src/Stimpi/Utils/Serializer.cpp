#include "stpch.h"
#include "Stimpi/Utils/Serializer.h"

namespace Stimpi
{
	DataBuilder* DataBuilder::BeginMap(const std::string& name)
	{
		m_Emitter << YAML::Key << name << YAML::Value;
		m_Emitter << YAML::BeginMap;
		return this;
	}

	DataBuilder* DataBuilder::EndMap()
	{
		m_Emitter << YAML::EndMap;
		return this;
	}

	DataBuilder* Serializer::CreateDataBuilder()
	{
		return new DataBuilder();
	}
}