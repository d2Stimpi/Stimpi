#pragma once

#include "Gui/NNode/NGraph.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class NGraphSerializer
	{
	public:
		NGraphSerializer(NGraph* graph);

		void Serialize(const std::string& filePath);
		bool Deseriealize(const std::string& filePath);

		// Graph component serialization
		//void SerializeVariable(YAML::Emitter& out, Variable* var);
		//bool DeserializeVariable(const YAML::Node& yamlNode, Variable* var);

		void SerializeNode(YAML::Emitter& out, NNode* node);
		bool DeserializeNode(const YAML::Node& yamlNode, NNode* node);

		void SerializePin(YAML::Emitter& out, NPin* pin);
		bool DeserializePin(const YAML::Node& node, NPin* pin);

		void SerializePinConnection(YAML::Emitter& out, NPinConnection* connection);
		bool DeserializePinConnection(const YAML::Node& node, NPinConnection* connection);

	private:
		std::shared_ptr<NPin> FindPinByID(NPinId ID);
		void ResetGraphData();

	private:
		NGraph* m_Graph;
		// For convenient pin search
		std::vector<std::shared_ptr<NPin>> m_Pins;
	};
}