#include "stpch.h"
#include "Gui/NNode/NGraphSerializer.h"

#include "Gui/Utils/YamlCustomTypesEditor.h"

#include "Stimpi/Log.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Utils/YamlCustomTypes.h"

namespace Stimpi
{
	static std::string NodeTypeToString(NNode::Type type)
	{
		switch (type)
		{
		case NNode::Type::None:		return "None";
		case NNode::Type::Getter:	return "Getter";
		case NNode::Type::Variable:	return "Variable";
		case NNode::Type::Modifier:	return "Modifier";
		case NNode::Type::Setter:	return "Setter";
		default: return "None";
		}
	}

	static NNode::Type StringToNodeType(const std::string& string)
	{
		if (string == "None")		return NNode::Type::None;
		if (string == "Getter")		return NNode::Type::Getter;
		if (string == "Variable")	return NNode::Type::Variable;
		if (string == "Modifier")	return NNode::Type::Modifier;
		if (string == "Setter")		return NNode::Type::Setter;

		ST_CORE_ASSERT("Grap load: Failed to parse Node data");
		return NNode::Type::None;
	}

	static std::string PinTypeToString(NPin::Type type)
	{
		switch (type)
		{
		case NPin::Type::None:	return "None";
		case NPin::Type::In:	return "In";
		case NPin::Type::Out:	return "Out";
		default: return std::string("UNKNOWN");
		}
	}

	static NPin::Type StringToPinType(const std::string& string)
	{
		if (string == "None")	return NPin::Type::None;
		if (string == "In")		return NPin::Type::In;
		if (string == "Out")	return NPin::Type::Out;

		ST_CORE_ASSERT("Grap load: Failed to parse Pin data");
		return NPin::Type::None;
	}

	static std::string PinDataTypeToString(NPin::DataType dataType)
	{
#define PIN_DATA_TYPE_TO_STRING_CASE(type)	case NPin::DataType::type:	return #type;

		switch (dataType)
		{
			PIN_DATA_TYPE_TO_STRING_CASE(Any);
			PIN_DATA_TYPE_TO_STRING_CASE(Entity);
			PIN_DATA_TYPE_TO_STRING_CASE(Bool);
			PIN_DATA_TYPE_TO_STRING_CASE(Int);
			PIN_DATA_TYPE_TO_STRING_CASE(Int2);
			PIN_DATA_TYPE_TO_STRING_CASE(Int3);
			PIN_DATA_TYPE_TO_STRING_CASE(Int4);
			PIN_DATA_TYPE_TO_STRING_CASE(Float);
			PIN_DATA_TYPE_TO_STRING_CASE(Float2);
			PIN_DATA_TYPE_TO_STRING_CASE(Float3);
			PIN_DATA_TYPE_TO_STRING_CASE(Float4);
		default: return "Any";
		}
	}

	static NPin::DataType StringToPinDataType(const std::string& string)
	{
#define STRING_TO_PIN_DATA_TYPE_STATEMENT(type)	if (string == #type)	return NPin::DataType::type;

		STRING_TO_PIN_DATA_TYPE_STATEMENT(Any);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Entity);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Bool);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Int);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Int2);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Int3);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Int4);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Float);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Float2);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Float3);
		STRING_TO_PIN_DATA_TYPE_STATEMENT(Float4);

		ST_CORE_ASSERT("Grap load: Failed to parse Variable data");
		return NPin::DataType::Any;
	}

	NGraphSerializer::NGraphSerializer(NGraph* graph)
		: m_Graph(graph)
	{

	}

	void NGraphSerializer::Serialize(const std::string& filePath)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Name" << YAML::Value << m_Graph->m_Name;
			out << YAML::Key << "UUID" << YAML::Value << m_Graph->m_ID;

			// TODO: variables

			// Nodes
			out << YAML::Key << "Nodes" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& node : m_Graph->m_Nodes)
			{
				SerializeNode(out, node.get());
			}
			out << YAML::EndMap;

			// Connections
			out << YAML::Key << "PinConnections" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& connection : m_Graph->m_PinConnections)
			{
				SerializePinConnection(out, connection.get());
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(filePath, out.c_str());
	}

	bool NGraphSerializer::Deseriealize(const std::string& filePath)
	{
		FilePath path = FilePath(filePath);
		if (!path.Exists())
		{
			ST_ERROR("Graph deserializer - file not found: {}", filePath);
			return false;
		}

		bool loaded = true;
		YAML::Node loadData = YAML::LoadFile(filePath);

		ResetGraphData();

		if (loadData["Name"])
		{
			m_Graph->m_Name = loadData["Name"].as<std::string>();
		}

		if (loadData["UUID"])
		{
			m_Graph->m_ID = loadData["UUID"].as<UUIDType>();
		}

		if (loadData["Nodes"])
		{
			YAML::Node nodes = loadData["Nodes"];
			for (YAML::const_iterator it = nodes.begin(); it != nodes.end(); it++)
			{
				YAML::Node node = it->second;
				std::shared_ptr<NNode> newNode = std::make_shared<NNode>("title", NNode::Type::None);
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
				std::shared_ptr<NPinConnection> newConnection = std::make_shared<NPinConnection>(nullptr, nullptr);
				DeserializePinConnection(connection, newConnection.get());
				m_Graph->m_PinConnections.emplace_back(newConnection);
			}
		}

		// Process PinConnections and fill out missing data
		for (auto connection : m_Graph->m_PinConnections)
		{
			auto src = FindPinByID(connection->m_Src->m_ID);
			auto dest = FindPinByID(connection->m_Dest->m_ID);

			src->m_ConnectedPins.push_back(dest);
			dest->m_ConnectedPins.push_back(src);
		}

		return loaded;
	}

	void NGraphSerializer::SerializeNode(YAML::Emitter& out, NNode* node)
	{
		ST_CORE_ASSERT(!node);

		out << YAML::Key << "Node";
		out << YAML::BeginMap;
		{
			// TODO: node types
			out << YAML::Key << "Position" << YAML::Value << node->m_Pos;
			out << YAML::Key << "Size" << YAML::Value << node->m_Size;
			out << YAML::Key << "UUID" << YAML::Value << node->m_ID;
			out << YAML::Key << "Title" << YAML::Value << node->m_Title;
			out << YAML::Key << "HasHeader" << YAML::Value << node->m_HasHeader;

			out << YAML::Key << "NodeType" << YAML::Value << NodeTypeToString(node->m_Type);

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

			// For less clutter of conversion to str method just use enum value
			out << YAML::Key << "MethodID" << YAML::Value << (uint32_t)node->m_MethodName;
		}
		out << YAML::EndMap;
	}

	bool NGraphSerializer::DeserializeNode(const YAML::Node& yamlNode, NNode* node)
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
		if (yamlNode["UUID"])
		{
			node->m_ID = yamlNode["UUID"].as<UUIDType>();
		}
		if (yamlNode["Title"])
		{
			node->m_Title = yamlNode["Title"].as<std::string>();
		}
		if (yamlNode["HasHeader"])
		{
			node->m_HasHeader = yamlNode["HasHeader"].as<bool>();
		}
		if (yamlNode["NodeType"])
		{
			node->m_Type = StringToNodeType(yamlNode["NodeType"].as<std::string>());
		}
		if (yamlNode["InputPins"])
		{
			YAML::Node inPins = yamlNode["InputPins"];
			for (YAML::const_iterator it = inPins.begin(); it != inPins.end(); it++)
			{
				YAML::Node pin = it->second;
				std::shared_ptr<NPin> newPin = std::make_shared<NPin>(nullptr, NPin::Type::None, "label", NPin::DataType::Any);
				DeserializePin(pin, newPin.get());
				newPin->m_ParentNode = node;
				node->m_InPins.emplace_back(newPin);
				// Add to Pin only collection
				m_Pins.emplace_back(newPin);
			}
		}
		if (yamlNode["OutputPins"])
		{
			YAML::Node outPins = yamlNode["OutputPins"];
			for (YAML::const_iterator it = outPins.begin(); it != outPins.end(); it++)
			{
				YAML::Node pin = it->second;
				std::shared_ptr<NPin> newPin = std::make_shared<NPin>(nullptr, NPin::Type::None, "label", NPin::DataType::Any);
				DeserializePin(pin, newPin.get());
				newPin->m_ParentNode = node;
				node->m_OutPins.emplace_back(newPin);
				// Add to Pin only collection
				m_Pins.emplace_back(newPin);
			}
		}
		if (yamlNode["MethodID"])
		{
			node->m_MethodName = (MethodName)yamlNode["MethodID"].as<uint32_t>();
		}

		return loaded;
	}

	void NGraphSerializer::SerializePin(YAML::Emitter& out, NPin* pin)
	{
		ST_CORE_ASSERT(!pin);

		out << YAML::Key << "Pin";
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Type" << YAML::Value << PinTypeToString(pin->m_Type);
			out << YAML::Key << "DataType" << YAML::Value << PinDataTypeToString(pin->m_DataType);
			out << YAML::Key << "UUID" << YAML::Value << pin->m_ID;
			out << YAML::Key << "Connected" << YAML::Value << pin->m_Connected;
			out << YAML::Key << "SingleConnecton" << YAML::Value << pin->m_SingleConnection;
			out << YAML::Key << "Label" << YAML::Value << pin->m_Label;
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

	bool NGraphSerializer::DeserializePin(const YAML::Node& node, NPin* pin)
	{
		bool loaded = true;
		ST_CORE_ASSERT(!pin);

		if (node["Type"])
		{
			pin->m_Type = StringToPinType(node["Type"].as<std::string>());
		}
		if (node["DataType"])
		{
			pin->m_DataType = StringToPinDataType(node["DataType"].as<std::string>());
		}
		if (node["UUID"])
		{
			pin->m_ID = node["UUID"].as<UUIDType>();
		}
		if (node["Connected"])
		{
			pin->m_Connected = node["Connected"].as<bool>();
		}
		if (node["SingleConnecton"])
		{
			pin->m_SingleConnection = node["SingleConnecton"].as<bool>();
		}
		if (node["Label"])
		{
			pin->m_Label = node["Label"].as<std::string>();
		}
		if (node["Position"])
		{
			YAML::Node view = node["Position"];
			pin->m_Pos = { view[0].as<float>(), view[1].as<float>() };
		}

		return loaded;
	}

	void NGraphSerializer::SerializePinConnection(YAML::Emitter& out, NPinConnection* connection)
	{
		ST_CORE_ASSERT(!connection);

		out << YAML::Key << "PinConnection";
		out << YAML::BeginMap;
		{
			out << YAML::Key << "SrcPin" << YAML::Value << connection->m_Src->m_ID;
			out << YAML::Key << "DestPin" << YAML::Value << connection->m_Dest->m_ID;
		}
		out << YAML::EndMap;
	}

	bool NGraphSerializer::DeserializePinConnection(const YAML::Node& node, NPinConnection* connection)
	{
		bool loaded = true;
		ST_CORE_ASSERT(!connection);

		if (node["SrcPin"])
		{
			UUIDType srcID = node["SrcPin"].as<UUIDType>();
			connection->m_Src = FindPinByID(srcID);
		}
		if (node["DestPin"])
		{
			UUIDType destID = node["DestPin"].as<UUIDType>();
			connection->m_Dest = FindPinByID(destID);
		}

		return loaded;
	}

	std::shared_ptr<NPin> NGraphSerializer::FindPinByID(NPinId ID)
	{
		auto found = std::find_if(m_Pins.begin(), m_Pins.end(), [&](auto pin) { return pin->m_ID == ID; });
		if (found != m_Pins.end())
			return *found;

		return nullptr;
	}

	void NGraphSerializer::ResetGraphData()
	{
		m_Graph->m_Nodes.clear();
		m_Graph->m_PinConnections.clear();
		m_Pins.clear();
	}

}