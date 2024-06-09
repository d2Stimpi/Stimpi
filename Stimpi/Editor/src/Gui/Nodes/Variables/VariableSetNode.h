#pragma once

#include "Gui/Nodes/CodeComponent.h"

namespace Stimpi
{
	struct Variable;

	class VariableSetNode : public CodeComponent
	{
	public:
		VariableSetNode(Node* node);
		~VariableSetNode();

		std::string GetValueName(Node* node) override;

		static Node* CreateNode(std::shared_ptr<Variable> var);

	private:
		Node* m_Node;
	};
}