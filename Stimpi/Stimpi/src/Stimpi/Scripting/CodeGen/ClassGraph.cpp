#include "stpch.h"
#include "ClassBuilder.h"

namespace Stimpi
{

	ClassGraph::ClassGraph(std::string name)
		: m_ClassName(name)
	{

	}

	ClassGraph::~ClassGraph()
	{

	}

	void ClassGraph::AddVariable(Expression* var)
	{
		m_PassiveExpressions.emplace_back(var);
	}

	void ClassGraph::AddMethod(Expression* method)
	{
		m_ExecutionExpressions.emplace_back(method);
	}

	void ClassGraph::AddStatement(Expression* statement)
	{
		m_ExecutionExpressions.emplace_back(statement);
	}

	void ClassGraph::AddEvent(Expression* statement)
	{
		m_ExecutionExpressions.emplace_back(statement);
	}

}