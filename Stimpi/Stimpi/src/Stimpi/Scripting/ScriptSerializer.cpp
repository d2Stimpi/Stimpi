#include "stpch.h"
#include "Stimpi/Scripting/ScriptSerializer.h"

#define ST_REGISTER_FIELD_TYPE_SERIALIZE(FieldTypeName, Func)		s_SerializeByTypeFunctions[FieldTypeName] = Func;
#define ST_REGISTER_FIELD_TYPE_DESERIALIZE(FieldTypeName, Func)		s_DeserializeByTypeFunctions[FieldTypeName] = Func;

namespace Stimpi
{

	std::unordered_map<std::string, std::function<void(YAML::Emitter&, ScriptObject*, ScriptField*)>> s_SerializeByTypeFunctions;
	std::unordered_map<std::string, std::function<void(const YAML::Node&, ScriptObject*, ScriptField*)>> s_DeserializeByTypeFunctions;

	static void EntitySerialize(YAML::Emitter& out, ScriptObject* ownerObj, ScriptField* field)
	{
		uint32_t fieldData = 0;
		auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), false);
		if (entityObj)
		{
			entityObj->GetFieldValue("ID", &fieldData);

			out << YAML::Key << "Field";
			out << YAML::BeginMap;
			{
				out << YAML::Key << "Type" << ScriptSeriaizer::FieldTypeToString(field->GetType());
				out << YAML::Key << "FieldTypeName" << field->GetFieldTypeName();
				out << YAML::Key << "Name" << field->GetName();
				out << YAML::Key << "FieldData";
				out << YAML::BeginMap;
				{
					out << YAML::Key << "ID" << fieldData;
				}
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}
	}

	static void EntityDeserialize(const YAML::Node& node, ScriptObject* ownerObj, ScriptField* field)
	{
		if (!node["FieldData"]["ID"])
			return;

		uint32_t id = node["FieldData"]["ID"].as<uint32_t>();
		auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), true);
		entityObj->SetFieldValue("ID", &id);
	}

	/* ======== ScriptSeriaizer generic functions ======== */

	static void CallSerializeByTypeFunction(YAML::Emitter& out, ScriptObject* ownerObj, ScriptField* field)
	{
		std::string fieldTypeName = field->GetFieldTypeName();
		if (s_SerializeByTypeFunctions.find(fieldTypeName) != s_SerializeByTypeFunctions.end())
		{
			s_SerializeByTypeFunctions.at(fieldTypeName)(out, ownerObj, field);
		}
	}

	static void CallDeserializeByTypeFunction(const YAML::Node& node, ScriptObject* ownerObj, ScriptField* field)
	{
		std::string fieldTypeName = field->GetFieldTypeName();
		if (s_DeserializeByTypeFunctions.find(fieldTypeName) != s_DeserializeByTypeFunctions.end())
		{
			s_DeserializeByTypeFunctions.at(fieldTypeName)(node, ownerObj, field);
		}
	}

	void ScriptSeriaizer::RegisterSirializableTypes()
	{
		ST_REGISTER_FIELD_TYPE_SERIALIZE("Stimpi.Entity", EntitySerialize);
		ST_REGISTER_FIELD_TYPE_DESERIALIZE("Stimpi.Entity", EntityDeserialize);
	}

	void ScriptSeriaizer::SerializeScriptField(YAML::Emitter& out, ScriptObject* ownerObj, ScriptField* field)
	{
		CallSerializeByTypeFunction(out, ownerObj, field);
	}

	void ScriptSeriaizer::DeserializeScriptField(const YAML::Node& node, ScriptObject* ownerObj, ScriptField* field)
	{
		CallDeserializeByTypeFunction(node, ownerObj, field);
	}

	std::string ScriptSeriaizer::FieldTypeToString(FieldType type)
	{
		switch (type)
		{
		case FieldType::UNKNOWN:			return "UNKNOWN";
		case FieldType::FIELD_TYPE_FLOAT:	return "FIELD_TYPE_FLOAT";
		case FieldType::FIELD_TYPE_INT:		return "FIELD_TYPE_INT";
		case FieldType::FIELD_TYPE_UINT:	return "FIELD_TYPE_UINT";
		case FieldType::FIELD_TYPE_CLASS:	return "FIELD_TYPE_CLASS";
		case FieldType::FIELD_TYPE_STRUCT:	return "FIELD_TYPE_STRUCT";
		}

		return "UNKNOWN";
	}

	Stimpi::FieldType ScriptSeriaizer::StringToFieldType(std::string type)
	{
		if (type == "FIELD_TYPE_FLOAT")		return FieldType::FIELD_TYPE_FLOAT;
		if (type == "FIELD_TYPE_INT")		return FieldType::FIELD_TYPE_INT;
		if (type == "FIELD_TYPE_UINT")		return FieldType::FIELD_TYPE_UINT;
		if (type == "FIELD_TYPE_CLASS")		return FieldType::FIELD_TYPE_CLASS;
		if (type == "FIELD_TYPE_STRUCT")	return FieldType::FIELD_TYPE_STRUCT;
		
		return FieldType::UNKNOWN;
	}

}