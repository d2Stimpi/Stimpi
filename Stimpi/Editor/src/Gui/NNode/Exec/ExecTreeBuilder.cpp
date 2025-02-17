#include "stpch.h"
#include "Gui/NNode/Exec/ExecTreeBuilder.h"

#include "Stimpi/Log.h"

namespace Stimpi
{
	static void VisitNode(NNode* node)
	{
		ST_INFO("[VisitNode] node: {}::{}", node->m_Title, node->m_ID);

		if (node->HasInputConnection())
		{
			for (auto& in : node->m_InPins)
			{
				if (in->m_Connected)
				{
					NNode* leafNode = in->m_ConnectedPins[0]->m_ParentNode;
					VisitNode(leafNode);
				}
			}
		}
	}


	std::shared_ptr<Stimpi::ExecTree> ExecTreeBuilder::BuildExecutionTree(NGraph* graph)
	{
		std::shared_ptr<ExecTree> tree = std::make_shared<ExecTree>();

		std::shared_ptr<NNode> root;
		// 1. Find final source node - "tree root"
		for (auto& node : graph->m_Nodes)
		{
			if (node->m_Type == NNode::Type::Setter)
			{
				root = node;
				// 2. Build ExecTree from this node
				//ProcessRootNode(tree.get(), root.get());

				VisitNode(root.get());
			}
		}

		return tree;
	}

	void ExecTreeBuilder::ProcessRootNode(ExecTree* tree, NNode* node)
	{
		// Check if the setter node has any connections
		if (node->HasInputConnection())
		{
			std::vector<uint32_t> inputs;
			uint32_t inputCnt = 0;

			// Add param data
			for (auto& in : node->m_InPins)
			{
				/*if (in->m_DataType == NPin::DataType::Float3)
				{
					tree->m_Params.emplace_back(glm::vec3(0.0));
				}*/

				tree->m_Params.emplace_back(glm::vec3(0.0));
				inputs.push_back(inputCnt++);

			}

			// Add method
			tree->m_Methods.emplace_back(inputs, true, node->m_MethodName, tree);
		}
	}

}