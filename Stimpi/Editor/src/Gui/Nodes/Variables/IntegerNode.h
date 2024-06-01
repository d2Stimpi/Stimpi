#pragma once

#include "Gui/Nodes/CodeComponent.h"

namespace Stimpi
{
	class Node;

	class IntegerNode : public CodeComponent
	{
	public:
		IntegerNode();

		std::string GetValueName(Node* node) override;

		static Node* CreateNode();
	};
}