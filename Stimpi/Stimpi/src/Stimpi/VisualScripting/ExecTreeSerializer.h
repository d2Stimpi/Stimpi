#pragma once

#include "Stimpi/VisualScripting/ExecTree.h"
#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	class ST_API ExecTreeSerializer
	{
	public:
		ExecTreeSerializer(ExecTree* execTree);

		void Serialize(const std::string& filePath);
		bool Deseriealize(const std::string& filePath);

		void SerializeParam(YAML::Emitter& out, const Param& param);
		bool DeserializeParam(const YAML::Node& yamlNode, Param* param);

		void SerializeMethod(YAML::Emitter& out, Method* method);
		std::shared_ptr<Method> DeserializeMethod(const YAML::Node& yamlNode);

	private:
		void ResetTreeData();

	private:
		ExecTree* m_ExecTree;
	};
}