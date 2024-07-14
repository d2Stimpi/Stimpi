#pragma once

#include "Stimpi/Scripting/ScriptEngine.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class ScriptSeriaizer
	{
	public:
		static void RegisterSirializableTypes();

		static void SerializeScriptField(YAML::Emitter& out, ScriptObject* ownerObj, ScriptField* field);
		static void DeserializeScriptField(const YAML::Node& node, ScriptObject* ownerObj, ScriptField* field);

		static std::string FieldTypeToString(FieldType type);
		static FieldType StringToFieldType(std::string type);
	};
}