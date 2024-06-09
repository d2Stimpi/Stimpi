#pragma once

#include "Gui/Nodes/CodeComponent.h"

namespace Stimpi
{
	class OnCreateNode : public CodeComponent
	{
	public:

		static Node* CreateNode();
		static std::string GetName();
	};
}