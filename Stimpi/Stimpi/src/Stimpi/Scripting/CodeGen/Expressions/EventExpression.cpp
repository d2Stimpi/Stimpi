#include "stpch.h"
#include "EventExpression.h"

namespace Stimpi
{

	EventExpression::EventExpression(std::string name)
		: Expression(ExpressionType::Event), m_Name(name)
	{
		Port* outPort = new Port(PortType::Bool);
		m_OutPort.reset(outPort);
	}

	void EventExpression::BuildDeclaration(CodeWriter* writer)
	{
		
	}

	void EventExpression::BuildExpression(CodeWriter* writer)
	{
		*writer << "if (Input.IsKeyPressed(KeyCode.KEY_SPACE))" << std::endl;
		// Body of the condition is generated in ClassBuilder
	}

}