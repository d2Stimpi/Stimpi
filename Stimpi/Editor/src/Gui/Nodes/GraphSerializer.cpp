#include "stpch.h"
#include "Gui/Nodes/GraphSerializer.h"

#include "Stimpi/Log.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Utils/YamlCustomTypes.h"

namespace Stimpi
{
	// Helpers
	static std::string PinTypeToString(Pin::Type type)
	{
		switch (type)
		{
		case Pin::Type::INPUT: return std::string("INPUT");
		case Pin::Type::OUTPUT: return std::string("OUTPUT");
		default: return std::string("UNKNOWN");
		}
	}

	static Pin::Type StringToPinType(const std::string& string)
	{
		if (string == "INPUT")	return Pin::Type::INPUT;
		if (string == "OUTPUT")	return Pin::Type::OUTPUT;

		ST_CORE_ASSERT("Grap load: Failed to parse Pin data");
		return Pin::Type::INPUT;
	}

	GraphSerializer::GraphSerializer(Graph* graph)
		: m_Graph(graph)
	{
	}

	void GraphSerializer::Serialize(const std::string& filePath)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Nodes" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& node : m_Graph->m_Nodes)
			{
				SerializeNode(out, node.get());
			}
			out << YAML::EndMap;

			out << YAML::Key << "PinConnections" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& connection : m_Graph->m_PinConnections)
			{
				SerializePinConnection(out, connection.get());
			}
			out << YAML::EndMap;
			out << YAML::Key << "NextNodeID" << YAML::Value << m_Graph->m_NextNodeID;
			out << YAML::Key << "NextPinID" << YAML::Value << m_Graph->m_NextPinID;
		}
		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(filePath, out.c_str());
	}

	bool GraphSerializer::Deseriealize(const std::string& filePath)
	{
		bool loaded = true;
		YAML::Node loadData = YAML::LoadFile(filePath);
		
		ResetGraphData();

		if (loadData["Nodes"])
		{
			YAML::Node nodes = loadData["Nodes"];
			for (YAML::const_iterator it = nodes.begin(); it != nodes.end(); it++)
			{
				YAML::Node node = it->second;
				// Node loading
				std::shared_ptr<Node> newNode = std::make_shared<Node>();
				DeserializeNode(node, newNode.get());
				m_Graph->m_Nodes.emplace_back(newNode);
			}
		}

		if (loadData["PinConnections"])
		{
			YAML::Node connectios = loadData["PinConnections"];
			for (YAML::const_iterator it = connectios.begin(); it != connectios.end(); it++)
			{
				YAML::Node connection = it->second;
				// PinConnection loading
				std::shared_ptr<PinConnection> newConnection = std::make_shared<PinConnection>();
				DeserializePinConnection(connection, newConnection.get());
				m_Graph->m_PinConnections.emplace_back(newConnection);
			}
		}

		if (loadData["NextNodeID"])
		{
			m_Graph->m_NextNodeID = loadData["NextNodeID"].as<uint32_t>();
		}

		if (loadData["NextPinID"])
		{
			m_Graph->m_NextPinID = loadData["NextPinID"].as<uint32_t>();
		}

		// Process PinConnections and fill out missing data
		for (auto connection : m_Graph->m_PinConnections)
		{
			Pin* src = FindPinByID(connection->m_SourcePin->m_ID);
			Pin* dest = FindPinByID(connection->m_DestinationPin->m_ID);

			src->m_ConnectedPins.emplace_back(dest);
			dest->m_ConnectedPins.emplace_back(src);
		}

		return loaded;
	}

	void GraphSerializer::SerializeNode(YAML::Emitter& out, Node* node)
	{
		ST_CORE_ASSERT(!node);

		out << YAML::Key << "Node";
		out << YAML::BeginMap;
		{
			// TODO: node types
			out << YAML::Key << "Position" << YAML::Value << node->m_Pos;
			out << YAML::Key << "Size" << YAML::Value << node->m_Size;
			out << YAML::Key << "ID" << YAML::Value << node->m_ID;
			out << YAML::Key << "Title" << YAML::Value << node->m_Title;
			
			out << YAML::Key << "InputPins" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& inPin : node->m_InPins)
			{
				SerializePin(out, inPin.get());
			}
			out << YAML::EndMap;

			out << YAML::Key << "OutputPins" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& outPin : node->m_OutPins)
			{
				SerializePin(out, outPin.get());
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}

	bool GraphSerializer::DeserializeNode(const YAML::Node& yamlNode, Node* node)
	{
		bool loaded = true;
		ST_CORE_ASSERT(!node);

		if (yamlNode["Position"])
		{
			node->m_Pos = yamlNode["Position"].as<ImVec2>();
		}
		if (yamlNode["Size"])
		{
			node->m_Size = yamlNode["Size"].as<ImVec2>();
		}
		if (yamlNode["ID"])
		{
			node->m_ID = yamlNode["ID"].as<uint32_t>();
		}
		if (yamlNode["Title"])
		{
			node->m_Title = yamlNode["Title"].as<std::string>();
		}
		if (yamlNode["InputPins"])
		{
			YAML::Node inPins = yamlNode["InputPins"];
			for (YAML::const_iterator it = inPins.begin(); it != inPins.end(); it++)
			{
				YAML::Node pin = it->second;
				// Pin loading
				std::shared_ptr<Pin> newPin = std::make_shared<Pin>();
				DeserializePin(pin, newPin.get());
				newPin->m_ParentNode = node;
				node->m_InPins.emplace_back(newPin);
				// Add to Pin only collection
				m_Graph->m_Pins.emplace_back(newPin);
			}
		}
		if (yamlNode["OutputPins"])
		{
			YAML::Node outPins = yamlNode["OutputPins"];
			for (YAML::const_iterator it = outPins.begin(); it != outPins.end(); it++)
			{
				YAML::Node pin = it->second;
				// Pin loading
				std::shared_ptr<Pin> newPin = std::make_shared<Pin>();
				DeserializePin(pin, newPin.get());
				newPin->m_ParentNode = node;
				node->m_OutPins.emplace_back(newPin);
				// Add to Pin only collection
				m_Graph->m_Pins.emplace_back(newPin);
			}
		}

		return loaded;
	}

	void GraphSerializer::SerializePin(YAML::Emitter& out, Pin* pin)
	{
		ST_CORE_ASSERT(!pin);

		out << YAML::Key << "Pin";
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Type" << YAML::Value << PinTypeToString(pin->m_Type);
			out << YAML::Key << "ID" << YAML::Value << pin->m_ID;
			out << YAML::Key << "Connected" << YAML::Value << pin->m_Connected;
			out << YAML::Key << "SingleConnecton" << YAML::Value << pin->m_SingleConnection;
			out << YAML::Key << "Text" << YAML::Value << pin->m_Text;
			out << YAML::Key << "Position" << YAML::Value;
			out << YAML::BeginSeq;
			{
				out << pin->m_Pos.x << pin->m_Pos.y;
			}
			out << YAML::EndSeq;

			//TODO: Connected Pins
		}
		out << YAML::EndMap;
	}

	bool GraphSerializer::DeserializePin(const YAML::Node& node, Pin* pin)
	{
		bool loaded = true;
		ST_CORE_ASSERT(!pin);

		if (node["Type"])
		{
			pin->m_Type = StringToPinType(node["Type"].as<std::string>());
		}
		if (node["ID"])
		{
			pin->m_ID = node["ID"].as<uint32_t>();
		}
		if (node["Connected"])
		{
			pin->m_Connected = node["Connected"].as<bool>();
		}
		if (node["SingleConnecton"])
		{
			pin->m_SingleConnection = node["SingleConnecton"].as<bool>();
		}
		if (node["Text"])
		{
			pin->m_Text = node["Text"].as<std::string>();
		}
		if (node["Position"])
		{
			YAML::Node view = node["Position"];
			pin->m_Pos = { view[0].as<float>(), view[1].as<float>() };
		}

		return loaded;
	}

	void GraphSerializer::SerializePinConnection(YAML::Emitter& out, PinConnection* connection)
	{
		ST_CORE_ASSERT(!connection);

		out << YAML::Key << "PinConnection";
		out << YAML::BeginMap;
		{
			out << YAML::Key << "SourcePin" << YAML::Value << connection->m_SourcePin->m_ID;
			out << YAML::Key << "DestinationPin" << YAML::Value << connection->m_DestinationPin->m_ID;
		}
		out << YAML::EndMap;
	}

	bool GraphSerializer::DeserializePinConnection(const YAML::Node& node, PinConnection* connection)
	{
		bool loaded = true;
		ST_CORE_ASSERT(!connection);

		if (node["SourcePin"])
		{
			uint32_t srcID = node["SourcePin"].as<uint32_t>();
			connection->m_SourcePin = FindPinByID(srcID);
		}
		if (node["DestinationPin"])
		{
			uint32_t destID = node["DestinationPin"].as<uint32_t>();
			connection->m_DestinationPin = FindPinByID(destID);
		}

		return loaded;
	}

	Stimpi::Pin* GraphSerializer::FindPinByID(uint32_t ID)
	{
		for (auto& pin : m_Graph->m_Pins)
		{
			if (pin->m_ID == ID)
				return pin.get();
		}

		return nullptr;
	}

	void GraphSerializer::ResetGraphData()
	{
		m_Graph->m_NextNodeID = 0;
		m_Graph->m_NextPinID = 0;
		m_Graph->m_Nodes.clear();
		m_Graph->m_PinConnections.clear();
		m_Graph->m_Pins.clear();
	}

}