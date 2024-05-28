#include "stpch.h"
#include "OnCreateExpression.h"

namespace Stimpi
{

	OnCreateExpression::OnCreateExpression()
		: Expression(ExpressionType::Create), m_Name("")
	{
		Port* outPort = new Port(PortType::Bool);
		m_OutPort.reset(outPort);
	}

	void OnCreateExpression::BuildDeclaration(CodeWriter* writer)
	{

	}

	void OnCreateExpression::BuildExpression(CodeWriter* writer)
	{

	}

}