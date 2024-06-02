#pragma once

#include "Gui/Nodes/CodeComponent.h"

namespace Stimpi
{
	struct Variable;

	class VariableGetNode : public CodeComponent
	{
	public:
		VariableGetNode(Node* node);
		~VariableGetNode();

		std::string GetValueName(Node* node) override;

		static Node* CreateNode(std::shared_ptr<Variable> var);
	
	private:
		Node* m_Node;
	};
}