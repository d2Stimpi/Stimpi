#include "stpch.h"
#include "VariableExpression.h"

namespace Stimpi
{

	VariableExpression::VariableExpression(const std::string& name, PortType type)
		: Expression(ExpressionType::Variable), m_Name(name), m_Port(std::make_shared<Port>(type))
	{
		m_Port->SetOwner(this);
	}

	VariableExpression::~VariableExpression()
	{

	}

	void VariableExpression::BuildDeclaration(CodeWriter* writer)
	{
		*writer << "public " << PortTypeToString(m_Port->GetType()) << " " << m_Name << ";" << std::endl;
	}

	void VariableExpression::BuildExpression(CodeWriter* writer)
	{
		
	}

	std::string VariableExpression::BuildLValueName()
	{
		return m_Name;
	}

}