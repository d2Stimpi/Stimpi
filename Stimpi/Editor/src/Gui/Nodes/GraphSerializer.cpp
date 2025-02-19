#include "stpch.h"
#include "Gui/Nodes/GraphSerializer.h"


#include "Stimpi/Log.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Utils/YamlCustomTypes.h"
#include "Gui/Utils/YamlCustomTypesEditor.h"

namespace Stimpi
{
	// Helpers
	static std::string VariableValueTypeToString(Variable::ValueType valueType)
	{
		switch (valueType)
		{
		case Variable::ValueType::None:		return "Node";
		case Variable::ValueType::Flow:		return "Flow";
		case Variable::ValueType::Bool:		return "Bool";
		case Variable::ValueType::Int:		return "Int";
		case Variable::ValueType::Float:	return "Float";
		case Variable::ValueType::Vector2:	return "Vector2";
		case Variable::ValueType::String:	return "String";
		default: return "None";
		}
	}

	static Variable::ValueType StringToVariableValueType(const std::string& string)
	{
		if (string == "Node")		return Variable::ValueType::None;
		if (string == "Flow")		return Variable::ValueType::Flow;
		if (string == "Bool")		return Variable::ValueType::Bool;
		if (string == "Int")		return Variable::ValueType::Int;
		if (string == "Float")		return Variable::ValueType::Float;
		if (string == "Vector2")	return Variable::ValueType::Vector2;
		if (string == "String")		return Variable::ValueType::String;

		ST_CORE_ASSERT("Grap load: Failed to parse Variable data");
		return Variable::ValueType::None;
	}

	static pin_type_variant DeserializeVariableValueBasedOnType(const YAML::Node& node, const Variable::ValueType& valueType)
	{
		switch (valueType)
		{
		case Variable::ValueType::None:		return 0;
		case Variable::ValueType::Flow:		return 0;
		case Variable::ValueType::Bool:		return node["Value"].as<bool>();
		case Variable::ValueType::Int:		return node["Value"].as<int>();
		case Variable::ValueType::Float:	return node["Value"].as<float>();
		case Variable::ValueType::Vector2:	return glm::vec2(node["Value"][0].as<float>(), node["Value"][1].as<float>()); ;
		case Variable::ValueType::String:	return node["Value"].as<std::string>();
		default: return 0;
		}
	}

	static std::string NodeTypeToString(Node::NodeType type)
	{
		switch (type)
		{
		case Node::NodeType::None:		return "None";
		case Node::NodeType::Variable:	return "Variable";
		case Node::NodeType::Property:	return "Property";
		case Node::NodeType::Method:	return "Method";
		case Node::NodeType::Statement:	return "Statement";
		case Node::NodeType::Event:		return "Event";
		case Node::NodeType::Create:	return "Create";
		default: return "None";
		}
	}

	static Node::NodeType StringToNodeType(const std::string& string)
	{
		if (string == "None")		return Node::NodeType::None;
		if (string == "Variable")	return Node::NodeType::Variable;
		if (string == "Property")	return Node::NodeType::Property;
		if (string == "Method")		return Node::NodeType::Method;
		if (string == "Statement")	return Node::NodeType::Statement;
		if (string == "Event")		return Node::NodeType::Event;
		if (string == "Create")		return Node::NodeType::Create;

		ST_CORE_ASSERT("Grap load: Failed to parse Node data");
		return Node::NodeType::None;
	}

	static std::string PinTypeToString(Pin::Type type)
	{
		switch (type)
		{
		case Pin::Type::FLOW_IN:	return std::string("FLOW_IN");
		case Pin::Type::INPUT:		return std::string("INPUT");
		case Pin::Type::FLOW_OUT:	return std::string("FLOW_OUT");
		case Pin::Type::OUTPUT:		return std::string("OUTPUT");
		default: return std::string("UNKNOWN");
		}
	}

	static Pin::Type StringToPinType(const std::string& string)
	{
		if (string == "FLOW_IN")	return Pin::Type::FLOW_IN;
		if (string == "INPUT")		return Pin::Type::INPUT;
		if (string == "FLOW_OUT")	return Pin::Type::FLOW_OUT;
		if (string == "OUTPUT")		return Pin::Type::OUTPUT;

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
			// Variables
			out << YAML::Key << "Variables" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& variable : m_Graph->m_Variables)
			{
				SerializeVariable(out, variable.get());
			}
			out << YAML::EndMap;

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

		if (loadData["Variables"])
		{
			YAML::Node variables = loadData["Variables"];
			for (YAML::const_iterator it = variables.begin(); it != variables.end(); it++)
			{
				YAML::Node var = it->second;
				// Variable loading
				std::shared_ptr<Variable> newVar = std::make_shared<Variable>();
				DeserializeVariable(var, newVar.get());
				m_Graph->m_Variables.emplace_back(newVar);
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

		// Update variables - pin attachments
		for (auto& variable : m_Graph->m_Variables)
		{
			for (auto& attachedPin : variable->m_AttachedToPins)
			{
				attachedPin->m_Variable = variable;
			}
		}

		return loaded;
	}

	void GraphSerializer::SerializeVariable(YAML::Emitter& out, Variable* var)
	{
		ST_CORE_ASSERT(!var);

		out << YAML::Key << "Variable";
		out << YAML::BeginMap;
		{
			out << YAML::Key << "ValueType" << YAML::Value << VariableValueTypeToString(var->m_ValueType);
			out << YAML::Key << "Name" << YAML::Value << var->m_Name;
			std::visit([&out](auto&& val)
			{
				out << YAML::Key << "Value" << YAML::Value << val;
			}, var->m_Value);
			out << YAML::Key << "ID" << YAML::Value << var->m_ID;
			
			out << YAML::Key << "AttachedPins" << YAML::Value;
			out << YAML::BeginMap;
			for (auto& attached : var->m_AttachedToPins)
			{
				out << YAML::Key << "AttachedPin" << YAML::Value;
				out << YAML::BeginMap;
				out << YAML::Key << "PinID" << YAML::Value << attached->m_ID;
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}

	bool GraphSerializer::DeserializeVariable(const YAML::Node& yamlNode, Variable* var)
	{
		bool loaded = true;
		ST_CORE_ASSERT(!var);

		if (yamlNode["ValueType"])
		{
			var->m_ValueType = StringToVariableValueType(yamlNode["ValueType"].as<std::string>());
		}
		if (yamlNode["Name"])
		{
			var->m_Name = yamlNode["Name"].as<std::string>();
		}
		if (yamlNode["Value"])
		{
			var->m_Value = DeserializeVariableValueBasedOnType(yamlNode, var->m_ValueType);
		}
		if (yamlNode["ID"])
		{
			var->m_ID = yamlNode["ID"].as<uint32_t>();
		}
		if (yamlNode["AttachedPins"])
		{
			YAML::Node attachedPins = yamlNode["AttachedPins"];
			for (YAML::const_iterator it = attachedPins.begin(); it != attachedPins.end(); it++)
			{
				YAML::Node pin = it->second;
				uint32_t pinID = pin["PinID"].as<uint32_t>();
				Pin* found = FindPinByID(pinID);
				if (found)
				{
					var->m_AttachedToPins.emplace_back(found);
				}
			}
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
			out << YAML::Key << "HasHeader" << YAML::Value << node->m_HasHeader;
			
			// CodeComponent
			
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
			out << YAML::Key << "Text" << YAML::Value << pin->m_Variable->m_Name;
			out << YAML::Key << "PinValueType" << YAML::Value << VariableValueTypeToString(pin->m_Variable->m_ValueType);
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
			pin->m_Variable->m_Name = node["Text"].as<std::string>();
		}
		if (node["PinValueType"])
		{
			pin->m_Variable->m_ValueType = StringToVariableValueType(node["PinValueType"].as<std::string>());
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