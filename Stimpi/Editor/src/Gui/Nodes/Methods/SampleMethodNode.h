#pragma once

#include "Gui/Nodes/CodeComponent.h"

namespace Stimpi
{
	class Node;

	class SampleMethodNode : public CodeComponent
	{
	public:

		static Node* CreateNode();
	};
}