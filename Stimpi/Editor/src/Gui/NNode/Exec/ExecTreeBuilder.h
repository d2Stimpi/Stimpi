#pragma once

#include "Gui/NNode/Exec/ExecTree.h"
#include "Gui/NNode/NGraph.h"

namespace Stimpi
{
	class ExecTreeBuilder
	{
	public:
		static std::shared_ptr<ExecTree> BuildExecutionTree(NGraph* graph);
		static void ProcessRootNode(ExecTree* tree, NNode* node);
	};
}