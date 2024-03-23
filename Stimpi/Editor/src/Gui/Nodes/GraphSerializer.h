#pragma once

#include "Gui/Nodes/GraphComponents.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class GraphSerializer
	{
	public:
		GraphSerializer(Graph* graph);

		void Serialize(const std::string& filePath);
		bool Deseriealize(const std::string& filePath);

		// Graph component serialization
		void SerializeNode(YAML::Emitter& out, Node* node);
		bool DeserializeNode(const YAML::Node& yamlNode, Node* node);

		void SerializePin(YAML::Emitter& out, Pin* pin);
		bool DeserializePin(const YAML::Node& node, Pin* pin);

		void SerializePinConnection(YAML::Emitter& out, PinConnection* connection);
		bool DeserializePinConnection(const YAML::Node& node, PinConnection* connection);

	private:
		Pin* FindPinByID(uint32_t ID);
		void ResetGraphData();

	private:
		Graph* m_Graph;
	};
}