#include "stpch.h"
#include "Gui/NNode/Exec/ExecTreeBuilder.h"

#include "Stimpi/Log.h"

namespace Stimpi
{
	using PinParamIndexRegistry = std::unordered_map<UUIDType, uint32_t>;

	struct ExecTreeBuilderContext
	{
		uint32_t m_InParamIndex = 0;
		uint32_t m_OutParamIndex = 0;
		uint32_t m_ParamCount = 0;

		std::shared_ptr<ExecTree> m_ActiveTree = nullptr;
	};

	ExecTreeBuilderContext s_Context;

	// Used to save pin's uuid - param index pair for easier tracking when visiting nodes
	PinParamIndexRegistry s_PinParamIndexRegistry;

	static std::shared_ptr<ExecTree> StartNewTreeBuild()
	{
		s_Context.m_ActiveTree = std::make_shared<ExecTree>();

		// Reset counters
		s_Context.m_InParamIndex = 0;
		s_Context.m_OutParamIndex = 0;
		s_Context.m_ParamCount = 0;

		s_PinParamIndexRegistry.clear();

		return s_Context.m_ActiveTree;
	}

	static void ProcessGetterNode(NNode* node, NNode* parent)
	{
		std::vector<uint32_t> outputs;

		// Output pin data
		for (auto& outPin : node->m_OutPins)
		{
			//TODO: reconsider these checks, instead mark visited node to avoid doubling params

			// skip pins that are not connected
			if (!outPin->m_Connected)
				continue;

			// skip pins that are not connected to parent node
			auto found = std::find_if(outPin->m_ConnectedPins.begin(), outPin->m_ConnectedPins.end(),
				[&](std::shared_ptr<NPin> cpin)
			{
				return cpin->m_ParentNode->m_ID == parent->m_ID;
			});
			if (found == outPin->m_ConnectedPins.end())
				continue;

			s_Context.m_ActiveTree->m_Params.emplace_back(glm::vec3(0.0f));
			s_PinParamIndexRegistry[outPin->m_ID] = s_Context.m_ParamCount;
			outputs.push_back(s_Context.m_ParamCount++);
		}

		// Method
		auto method = std::shared_ptr<Method>(new Method(outputs, false, node->m_MethodName, s_Context.m_ActiveTree.get()));
		s_Context.m_ActiveTree->m_Methods.push_back(method);
	}

	static void ProcessModifierNode(NNode* node, NNode* parent)
	{
		std::vector<uint32_t> inputs;
		std::vector<uint32_t> outputs;

		for (auto& inPin : node->m_InPins)
		{
			if (inPin->m_Connected)
			{
				uint32_t inIndex = s_PinParamIndexRegistry.at(inPin->m_ConnectedPins[0]->m_ID);
				inputs.push_back(inIndex);
			}
			else
			{
				s_Context.m_ActiveTree->m_Params.emplace_back(glm::vec3(5.0f));
				inputs.push_back(s_Context.m_ParamCount++);
			}
		}

		for (auto& outPin : node->m_OutPins)
		{
			s_Context.m_ActiveTree->m_Params.emplace_back(glm::vec3(0.0f));
			s_PinParamIndexRegistry[outPin->m_ID] = s_Context.m_ParamCount;
			outputs.push_back(s_Context.m_ParamCount++);
		}

		auto method = std::shared_ptr<Method>(new Method(inputs, outputs, node->m_MethodName, s_Context.m_ActiveTree.get()));
		s_Context.m_ActiveTree->m_Methods.push_back(method);
	}

	static void ProcessSetterNode(NNode* node, NNode* parent)
	{
		std::vector<uint32_t> inputs;

		for (auto& inPin : node->m_InPins)
		{
			uint32_t inIndex = s_PinParamIndexRegistry.at(inPin->m_ConnectedPins[0]->m_ID);
			inputs.push_back(inIndex);
		}

		auto method = std::shared_ptr<Method>(new Method(inputs, true, node->m_MethodName, s_Context.m_ActiveTree.get()));
		s_Context.m_ActiveTree->m_Methods.push_back(method);
	}

	static void ProcessVaraibaleNode(NNode* node, NNode* parent)
	{

	}

	static void ProcessNode(NNode* node, NNode* parent)
	{
		switch (node->m_Type)
		{
		case NNode::Type::Getter:	ProcessGetterNode(node, parent);	break;
		case NNode::Type::Variable:	ProcessVaraibaleNode(node, parent); break;
		case NNode::Type::Modifier: ProcessModifierNode(node, parent);	break;
		case NNode::Type::Setter:	ProcessSetterNode(node, parent);	break;
		case NNode::Type::None:	//TODO: report error, should not get here
		default:
			break;
		}
	}

	// Node is visited by walking back from root (Setter) node by input pin connection 
	static void VisitNode(NNode* node, NNode* parent)
	{
		ST_INFO("[VisitNode] node: {}::{}", node->m_Title, node->m_ID);
		ST_INFO("[VisitNode] [node: {} <=> {}]", node->m_Title, parent != nullptr ? parent->m_Title : "n/a");

		if (node->HasInputConnection())
		{
			for (auto& in : node->m_InPins)
			{
				if (in->m_Connected)
				{
					NNode* leafNode = in->m_ConnectedPins[0]->m_ParentNode;
					VisitNode(leafNode, node);
				}
			}

			ST_INFO("[VisitNode] after visit node: {}::{}", node->m_Title, node->m_ID);
			ST_INFO("[VisitNode] [after visit node: {} <=> {}]", node->m_Title, parent != nullptr ? parent->m_Title : "n/a");

			ProcessNode(node, parent);
		}
		else
		{
			// Leaf node (Getter)
			ST_INFO("[VisitNode] leaf node: {}::{}", node->m_Title, node->m_ID);
			ST_INFO("[VisitNode] [leaf node: {} <=> {}]", node->m_Title, parent != nullptr ? parent->m_Title : "n/a");

			ProcessNode(node, parent);
		}
	}


	std::shared_ptr<Stimpi::ExecTree> ExecTreeBuilder::BuildExecutionTree(NGraph* graph)
	{
		std::shared_ptr<ExecTree> tree = StartNewTreeBuild();

		// 1. Find final source node - "tree root"
		for (auto& node : graph->m_Nodes)
		{
			if (node->m_Type == NNode::Type::Setter)
			{
				// 2. Build ExecTree from this node by walking the tree
				VisitNode(node.get(), nullptr);
			}
		}

		return tree;
	}

}