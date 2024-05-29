#pragma once

#include "CodeWriter.h"

#include "Gui/Nodes/GraphComponents.h"

namespace Stimpi
{
	class CodeGenerators
	{
	public:
		static void InitializeGenerators();
		// Common
		static void GenerateDeclaration(std::string name, Node* node, CodeWriter* writer);
		static void GenerateExpression(std::string name, Node* node, CodeWriter* writer);

		// Used by "passive" elements
		static std::string GetValueName(Node* node);
		static std::string GetInvokeStatement(Node* node);
	};
}