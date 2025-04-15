#include "stpch.h"
#include "Stimpi/VisualScripting/ExecTreeSerializer.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <yaml-cpp/yaml.h>

#define GET_PARAM_TYPE_METHOD(type)	\
	static std::string GetParamType(type param)	\
	{	\
	return #type;	\
	}

namespace Stimpi
{
	// Param variant methods
	GET_PARAM_TYPE_METHOD(uint32_t)
	GET_PARAM_TYPE_METHOD(bool)
	GET_PARAM_TYPE_METHOD(int)
	GET_PARAM_TYPE_METHOD(float)
	GET_PARAM_TYPE_METHOD(glm::vec2)
	GET_PARAM_TYPE_METHOD(glm::vec3)
	GET_PARAM_TYPE_METHOD(glm::vec4)
	GET_PARAM_TYPE_METHOD(UUID)

	ExecTreeSerializer::ExecTreeSerializer(ExecTree* execTree)
		: m_ExecTree(execTree)
	{
	}

	void ExecTreeSerializer::Serialize(const std::string& filePath)
	{
		if (m_ExecTree == nullptr)
		{
			ST_ERROR("[ExecTreeSerializer] error serializing nullptr data!");
			return;
		}

		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			// Params
			out << YAML::Key << "Params" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& param : m_ExecTree->m_Params)
			{
				SerializeParam(out, param);
			}
			out << YAML::EndMap;

			// Methods
			out << YAML::Key << "Methods" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& method : m_ExecTree->m_Methods)
			{
				SerializeMethod(out, method.get());
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(filePath, out.c_str());
	}

	bool ExecTreeSerializer::Deseriealize(const std::string& filePath)
	{
		FilePath path = FilePath(filePath);
		if (!path.Exists())
		{
			ST_ERROR("ExecTreeSerializer - file not found: {}", filePath);
			return false;
		}

		ResetTreeData();

		bool loaded = true;
		YAML::Node loadData = YAML::LoadFile(filePath);

		if (loadData["Params"])
		{
			YAML::Node nodes = loadData["Params"];
			for (YAML::const_iterator it = nodes.begin(); it != nodes.end(); it++)
			{
				YAML::Node node = it->second;
				Param param;
				DeserializeParam(node, &param);
				m_ExecTree->m_Params.push_back(param);
			}
		}

		if (loadData["Methods"])
		{
			YAML::Node connectios = loadData["Methods"];
			for (YAML::const_iterator it = connectios.begin(); it != connectios.end(); it++)
			{
				YAML::Node node = it->second;
				std::shared_ptr<Method> newMethod = DeserializeMethod(node);
				m_ExecTree->m_Methods.push_back(newMethod);
			}
		}

		return true;
	}

	void ExecTreeSerializer::SerializeParam(YAML::Emitter& out, const Param& param)
	{
		out << YAML::Key << "Param";
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Type" << YAML::Value << std::visit([](auto&& arg) { return GetParamType(arg); }, param);
			out << YAML::Key << "Value" << YAML::Value;

#define CHECK_GET_VARIANT_PARAM_DATA(type)		if (std::holds_alternative<type>(param)) out << std::get<type>(param)

			CHECK_GET_VARIANT_PARAM_DATA(uint32_t);
			CHECK_GET_VARIANT_PARAM_DATA(bool);
			CHECK_GET_VARIANT_PARAM_DATA(int);
			CHECK_GET_VARIANT_PARAM_DATA(float);
			CHECK_GET_VARIANT_PARAM_DATA(UUID);

			// Special case - TODO: define glm::vec4 yaml methods
			if (std::holds_alternative<glm::vec4>(param))
			{
				glm::vec4 val = std::get<glm::vec4>(param);
				out << YAML::BeginSeq;
				out << val.x << val.y << val.z << val.w;
				out << YAML::EndSeq;
			}

			// Special case - TODO: define glm::vec3 yaml methods
			if (std::holds_alternative<glm::vec3>(param))
			{
				glm::vec3 val = std::get<glm::vec3>(param);
				out << YAML::BeginSeq;
				out << val.x << val.y << val.z;
				out << YAML::EndSeq;
			}

			// Special case - TODO: define glm::vec2 yaml methods
			if (std::holds_alternative<glm::vec2>(param))
			{
				glm::vec2 val = std::get<glm::vec2>(param);
				out << YAML::BeginSeq;
				out << val.x << val.y;
				out << YAML::EndSeq;
			}

		}
		out << YAML::EndMap;
	}

	bool ExecTreeSerializer::DeserializeParam(const YAML::Node& yamlNode, Param* param)
	{
		bool loaded = true;
		ST_CORE_ASSERT(!param);

		std::string typeStr = "";
		if (yamlNode["Type"])
		{
			typeStr = yamlNode["Type"].as<std::string>();
		}
		if (yamlNode["Value"])
		{
#define STR_TO_VARIANT_PARAM_DATA(type)		if (typeStr == #type)	*param = yamlNode["Value"].as<type>()

			STR_TO_VARIANT_PARAM_DATA(uint32_t);
			STR_TO_VARIANT_PARAM_DATA(bool);
			STR_TO_VARIANT_PARAM_DATA(int);
			STR_TO_VARIANT_PARAM_DATA(float);

			// Special case - TODO: define glm::vec4 yaml methods
			if (typeStr == "glm::vec4")
			{
				YAML::Node vecNode = yamlNode["Value"];
				*param = glm::vec4(vecNode[0].as<float>(), vecNode[1].as<float>(), vecNode[2].as<float>(), vecNode[3].as<float>());
			}

			// Special case - TODO: define glm::vec3 yaml methods
			if (typeStr == "glm::vec3")
			{
				YAML::Node vecNode = yamlNode["Value"];
				*param = glm::vec3(vecNode[0].as<float>(), vecNode[1].as<float>(), vecNode[2].as<float>());
			}

			// Special case - TODO: define glm::vec2 yaml methods
			if (typeStr == "glm::vec2")
			{
				YAML::Node vecNode = yamlNode["Value"];
				*param = glm::vec2(vecNode[0].as<float>(), vecNode[1].as<float>());
			}
		}

		return true;
	}

	void ExecTreeSerializer::SerializeMethod(YAML::Emitter& out, Method* method)
	{
		ST_CORE_ASSERT(!method);

		out << YAML::Key << "Method";
		out << YAML::BeginMap;
		{
			out << YAML::Key << "InParams" << YAML::Value;
			out << YAML::BeginSeq;
			for (auto& inParam : method->m_InParams)
			{
				out << inParam;
			}
			out << YAML::EndSeq;


			out << YAML::Key << "OutParams" << YAML::Value;
			out << YAML::BeginSeq;
			for (auto& outParam : method->m_OutParams)
			{
				out << outParam;
			}
			out << YAML::EndSeq;

			out << YAML::Key << "MethodID" << YAML::Value << NNodeMethodRegistry::MethodNameToString(method->m_Name);
		}
		out << YAML::EndMap;
	}

	std::shared_ptr<Method> ExecTreeSerializer::DeserializeMethod(const YAML::Node& yamlNode)
	{
		std::vector<uint32_t> inParams;
		bool hasInParams = false;
		std::vector<uint32_t> outParams;
		bool hasOutParams = false;
		MethodName name;

		if (yamlNode["InParams"])
		{
			YAML::Node nodeInParams = yamlNode["InParams"];
			for (YAML::const_iterator it = nodeInParams.begin(); it != nodeInParams.end(); it++)
			{
				inParams.emplace_back(it->as<uint32_t>());
			}
			hasInParams = nodeInParams.size() > 0;
		}

		if (yamlNode["OutParams"])
		{
			YAML::Node nodeOutParams = yamlNode["OutParams"];
			for (YAML::const_iterator it = nodeOutParams.begin(); it != nodeOutParams.end(); it++)
			{
				outParams.emplace_back(it->as<uint32_t>());
			}
			hasOutParams = nodeOutParams.size() > 0;
		}

		if (yamlNode["MethodID"])
		{
			name = NNodeMethodRegistry::StringToMethodName(yamlNode["MethodID"].as<std::string>());
		}

		if (hasInParams && hasOutParams)
			return std::make_shared<Method>(inParams, outParams, name, m_ExecTree);
		else if (hasInParams)
			return std::make_shared<Method>(inParams, true, name, m_ExecTree);
		else if (hasOutParams)
			return std::make_shared<Method>(outParams, false, name, m_ExecTree);

		return nullptr;
	}

	void ExecTreeSerializer::ResetTreeData()
	{
		m_ExecTree->m_Entity = 0;
		m_ExecTree->m_Params.clear();
		m_ExecTree->m_Methods.clear();
	}

}