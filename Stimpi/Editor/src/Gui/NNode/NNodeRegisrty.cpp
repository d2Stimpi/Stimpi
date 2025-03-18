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

	static std::shared_ptr<NNode> GetSize(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Getter);
		node->AddPin(NPin::Type::Out, "Size", NPin::DataType::Float2);
		node->AddMethod(MethodName::GetSize);
		node->m_HasHeader = false;

		return node;
	}

	static std::shared_ptr<NNode> GetAnimationComponent(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Getter);
		node->AddPin(NPin::Type::Out, "AnimSprite", NPin::DataType::AnimSprite);
		node->AddPin(NPin::Type::Out, "DefaultAnimation", NPin::DataType::Animation);
		node->AddPin(NPin::Type::Out, "Animations", NPin::DataType::Animation);	// TODO: handle arrays
		node->AddPin(NPin::Type::Out, "Material", NPin::DataType::Materia);
		node->AddMethod(MethodName::GetAnimationComponent);
		node->m_HasHeader = true;

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

	static std::shared_ptr<NNode> Add(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "A", NPin::DataType::Float);
		node->AddPin(NPin::Type::In, "B", NPin::DataType::Float);
		node->AddPin(NPin::Type::Out, "", NPin::DataType::Float);
		node->AddMethod(MethodName::Add);

		return node;
	}

	static std::shared_ptr<NNode> Subtract(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "A", NPin::DataType::Float);
		node->AddPin(NPin::Type::In, "B", NPin::DataType::Float);
		node->AddPin(NPin::Type::Out, "", NPin::DataType::Float);
		node->AddMethod(MethodName::Subtract);

		return node;
	}

	static std::shared_ptr<NNode> Divide(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "A", NPin::DataType::Float);
		node->AddPin(NPin::Type::In, "B", NPin::DataType::Float);
		node->AddPin(NPin::Type::Out, "", NPin::DataType::Float);
		node->AddMethod(MethodName::Divide);

		return node;
	}

	static std::shared_ptr<NNode> Multiply(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "A", NPin::DataType::Float);
		node->AddPin(NPin::Type::In, "B", NPin::DataType::Float);
		node->AddPin(NPin::Type::Out, "", NPin::DataType::Float);
		node->AddMethod(MethodName::Multiply);

		return node;
	}

	static std::shared_ptr<NNode> Vector2(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "Vec2", NPin::DataType::Float2);
		node->AddPin(NPin::Type::Out, "X", NPin::DataType::Float);
		node->AddPin(NPin::Type::Out, "Y", NPin::DataType::Float);
		node->AddMethod(MethodName::Vector2);

		return node;
	}

	static std::shared_ptr<NNode> AnimSpriteMod(const std::string& title)
	{
		auto node = std::make_shared<NNode>("AnimSprite", NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "In", NPin::DataType::AnimSprite);
		node->AddPin(NPin::Type::Out, "Animation", NPin::DataType::Animation);
		node->AddPin(NPin::Type::Out, "PlaybackSpeed", NPin::DataType::Float);
		node->AddMethod(MethodName::AnimSpriteMod);

		return node;
	}

	static std::shared_ptr<NNode> AnimationMod(const std::string& title)
	{
		auto node = std::make_shared<NNode>("Animation", NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "In", NPin::DataType::Animation);
		node->AddPin(NPin::Type::Out, "SubTexture", NPin::DataType::SubTexture);
		node->AddMethod(MethodName::AnimationMod);

		return node;
	}

	static std::shared_ptr<NNode> SubTextureMod(const std::string& title)
	{
		auto node = std::make_shared<NNode>("SubTexture", NNode::Type::Modifier);
		node->AddPin(NPin::Type::In, "In", NPin::DataType::SubTexture);
		node->AddPin(NPin::Type::Out, "UV", NPin::DataType::Float2);
		node->AddMethod(MethodName::SubTextureMod);

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