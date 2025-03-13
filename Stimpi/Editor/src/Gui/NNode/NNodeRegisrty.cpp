#include "stpch.h"
#include "Gui/NNode/NNodeRegistry.h"

#define ST_REGISTER_NODE(Name, FuncName) s_NodeNames.emplace_back(Name); s_NodeRegistry[Name] = [](void* data) { return FuncName(Name); };

namespace Stimpi
{
	using NNodeBuilderFunc = std::function<std::shared_ptr<NNode>(void* data)>;
	using NNodeRegistryType = std::unordered_map<std::string, NNodeBuilderFunc>;

	std::vector<std::string> s_NodeNames;
	NNodeRegistryType s_NodeRegistry;

	/* Node construction methods begin */

	/* Getters */

	// TODO: Get entity by name makes more sense for usage of this getter
	static std::shared_ptr<NNode> GetEntity(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Getter);
		node->AddPin(NPin::Type::Out, "Entity", NPin::DataType::Entity);
		// TODO: AddMethod
		node->m_HasHeader = false;

		return node;
	}

	static std::shared_ptr<NNode> GetPosition(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Getter);
		node->AddPin(NPin::Type::Out, "Position", NPin::DataType::Float3);
		node->AddMethod(MethodName::GetPosition);
		node->m_HasHeader = false;

		return node;
	}

	static std::shared_ptr<NNode> GetMaterial(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Getter);
		node->AddPin(NPin::Type::Out, "Material", NPin::DataType::Material);
		// TODO: AddMethod
		node->m_HasHeader = false;

		return node;
	}

	/* Modifiers */
	static std::shared_ptr<NNode> Translate(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "Position", NPin::DataType::Float3);
		node->AddPin(NPin::Type::In, "Vec3", NPin::DataType::Float3);
		node->AddPin(NPin::Type::Out, "Output", NPin::DataType::Float3);
		node->AddMethod(MethodName::Translate);

		return node;
	}

	/* Setters */
	static std::shared_ptr<NNode> SetPosition(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Setter);
		node->AddPin(NPin::Type::In, "Position", NPin::DataType::Float3);
		node->AddMethod(MethodName::SetPosition);

		return node;
	}

	static std::shared_ptr<NNode> SetShaderData(const std::string& title)
	{
		return nullptr;
	}

	static std::shared_ptr<NNode> SetMaterialData(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Setter);

		return node;
	}

	/* Node construction methods end */

	void NNodeRegistry::InitializeNodeRegisrty()
	{
#define DEFINE_NODE_NAME(name)	ST_REGISTER_NODE(#name, name);
#include "Stimpi/VisualScripting/NNodeNames.h"
#undef DEFINE_NODE_NAME(name)
	}

	std::vector<std::string> NNodeRegistry::GetNodeNamesList()
	{
		return s_NodeNames;
	}

	std::shared_ptr<Stimpi::NNode> NNodeRegistry::CreateNodeByName(const std::string& name)
	{
		std::shared_ptr<NNode> node = nullptr;

		auto found = s_NodeRegistry.find(name);
		if (found != s_NodeRegistry.end())
			node = found->second(nullptr);

		if (node)
			node->CalcNodeSize();

		return node;
	}
}