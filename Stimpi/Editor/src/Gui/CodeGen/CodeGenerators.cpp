#include "stpch.h"
#include "CodeGenerators.h"

#include "Gui/Nodes/NodeNames.h"

namespace Stimpi
{
	std::unordered_map<std::string, std::function<void(Node*, CodeWriter*)>> s_DeclarationGenerators;
	std::unordered_map<std::string, std::function<void(Node*, CodeWriter*)>> s_ExpressionGenerators;
	
#pragma region DeclarationGenerators

	static void GenerateOnCreateDeclaration(Node* node, CodeWriter* wr)
	{

	}

	static void GenerateEventDeclaration(Node* node, CodeWriter* wr)
	{

	}

	static void GenerateMethodDeclaration(Node* node, CodeWriter* wr)
	{

	}

	static void GenerateVariableDeclaration(Node* node, CodeWriter* wr)
	{

	}

#pragma endregion DeclarationGenerators

#pragma region ExpressionGenerators

	static void GenerateOnCreateExpression(Node* node, CodeWriter* wr)
	{

	}

	static void GenerateEventExpression(Node* node, CodeWriter* wr)
	{

	}

	static void GenerateMethodExpression(Node* node, CodeWriter* wr)
	{

	}

	static void GenerateVariableExpression(Node* node, CodeWriter* wr)
	{

	}

#pragma endregion ExpressionGenerators


	void CodeGenerators::InitializeGenerators()
	{
		s_DeclarationGenerators = {
			{ ONCREATE_NODE,	nullptr },
			{ EVENT_NODE,		nullptr },
			{ METHOD_NODE,		nullptr },
			{ VARIABLE_NODE,	nullptr }
		};

		s_ExpressionGenerators = {
			{ ONCREATE_NODE,	nullptr },
			{ EVENT_NODE,		nullptr },
			{ METHOD_NODE,		nullptr },
			{ VARIABLE_NODE,	nullptr }
		};
	}

	void CodeGenerators::GenerateDeclaration(std::string name, Node* node, CodeWriter* writer)
	{
		auto generator = s_DeclarationGenerators[name];
		if (generator)
		{
			generator(node, writer);
		}
	}

	void CodeGenerators::GenerateExpression(std::string name, Node* node, CodeWriter* writer)
	{
		auto generator = s_ExpressionGenerators[name];
		if (generator)
		{
			generator(node, writer);
		}
	}

	std::string CodeGenerators::GetValueName(Node* node)
	{
		return "";
	}

	std::string CodeGenerators::GetInvokeStatement(Node* node)
	{
		return "";
	}

}