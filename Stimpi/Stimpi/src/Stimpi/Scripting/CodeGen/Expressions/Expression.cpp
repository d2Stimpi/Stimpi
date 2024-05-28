#include "stpch.h"
#include "Expression.h"

namespace Stimpi
{

	Expression::Expression(ExpressionType type)
		: m_Type(type)
	{

	}

	Expression::~Expression()
	{

	}

	std::string PortTypeToString(PortType portType)
	{
		switch (portType)
		{
		case PortType::Bool:		return "bool";
		case PortType::Int:			return "int";
		case PortType::Vector2:		return "Vector2";
		}

		//Error - invalid type value
		return "int";
	}

	Port::Port(PortType type)
		: m_Type(type)
	{
		
	}

	void Port::Connect(std::shared_ptr<Port> source)
	{
		// TODO: check if exists and add if new
		m_ConnectedPorts.emplace_back(source);
	}

}