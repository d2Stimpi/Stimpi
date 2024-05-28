#pragma once

#include "Stimpi/Core/Core.h"
#include "Expressions/Expression.h"


namespace Stimpi
{
	class ST_API ClassGraph
	{
	public:
		ClassGraph(std::string name);
		~ClassGraph();

		// Passive
		void AddVariable(Expression* var);

		// Active
		void AddMethod(Expression* method);
		void AddStatement(Expression* statement);
		void AddEvent(Expression* statement);

	private:
		std::vector<std::shared_ptr<Expression>> m_PassiveExpressions;	// execution flow independent, fields, properties, enums, etc...
		std::vector<std::shared_ptr<Expression>> m_ExecutionExpressions;	// execution flow dependent, functions, logical blocks, etc... 

		std::string m_ClassName;

		friend class ClassBuilder;
	};
}