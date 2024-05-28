#include "stpch.h"
#include "MethodExpression.h"

namespace Stimpi
{

	MethodExpression::MethodExpression(const std::string& name, MethodPortLayout layout)
		: Expression(ExpressionType::Method), m_Name(name)
	{
		for (auto params : layout)
		{
			Port* port = new Port(params.m_PortType);
			port->SetOwner(this);

			if (params.m_PortDirection == PortDirection::Input)
			{
				m_InPort.emplace_back(port);
			}
			else
			{
				m_OutPort.emplace_back(port);
			}
		}
	}

	std::shared_ptr<Stimpi::Port> MethodExpression::GetInPort(uint32_t index)
	{
		if (m_InPort.size() > index)
			return m_InPort[index];
		else
			return nullptr;
	}

	std::shared_ptr<Stimpi::Port> MethodExpression::GetOutPort(uint32_t index)
	{
		return m_OutPort[index];
	}

	void MethodExpression::BuildDeclaration(CodeWriter* writer)
	{
		*writer << "public void " << m_Name << "()" << std::endl;
		// Body start
		*writer << CodeManip::BlockBegin;

		BuildExpression(writer);

		// Body end
		*writer << CodeManip::BlockEnd;
	}

	void MethodExpression::BuildExpression(CodeWriter* writer)
	{
		if (!m_InPort.empty())
		{
			if (!m_InPort[0]->GetConnectedPorts().empty())
			{
				auto connectedPort = m_InPort[0]->GetConnectedPorts()[0];
				if (connectedPort)
				{
					// Assume here that we know what we have here. The exact port number and their types
					// TODO: value
					*writer << connectedPort->GetOwner()->BuildLValueName() << " = 20;" << std::endl;
				}
			}
		}
	}

	void MethodExpression::LinkToInputPort(std::shared_ptr<Port> source, std::shared_ptr<Port> target)
	{
		// Check if Port types are compatible
		if (source->GetType() != target->GetType())
		{
			std::cout << "Ports are not compatible for connection!" << std::endl;
			return;
		}

		source->Connect(target);
		target->Connect(source);
	}

	void MethodExpression::LinkToInputPort(std::shared_ptr<Port> source, uint32_t targetIndex)
	{
		LinkToInputPort(source, m_InPort[targetIndex]);
	}

}