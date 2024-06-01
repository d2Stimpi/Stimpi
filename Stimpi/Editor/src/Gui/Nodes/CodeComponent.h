#pragma once

namespace Stimpi
{
	class Node;
	class CodeWriter;

	class CodeComponent
	{
	public:
		CodeComponent();

		virtual void GenerateDeclaration(Node* node, CodeWriter* writer) {};
		virtual void GenerateExpression(Node* node, CodeWriter* writer) {};

		// Used by "passive" elements
		virtual std::string GetValueName(Node* node) { return ""; };
		virtual std::string GetInvokeStatement(Node* node) { return ""; };

	};
}