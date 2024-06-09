#pragma once

#include "Gui/Nodes/CodeComponent.h"

namespace Stimpi
{
	class SetPositionNode : public CodeComponent
	{
	public:
		void GenerateExpression(Node* node, CodeWriter* writer) override;

		static Node* CreateNode();
	};
}