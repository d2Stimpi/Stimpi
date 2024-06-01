#pragma once

#include "Gui/Nodes/CodeComponent.h"

namespace Stimpi
{
	class Node;

	class SampleMethodNode : public CodeComponent
	{
	public:
		//std::string GetValueName(Node* node) override;

		static Node* CreateNode();
	};
}