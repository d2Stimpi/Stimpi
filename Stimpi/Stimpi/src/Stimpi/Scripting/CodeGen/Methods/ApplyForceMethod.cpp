#include "stpch.h"
#include "ApplyForceMethod.h"

namespace Stimpi
{

	ApplyForceMethod::ApplyForceMethod()
		: MethodExpression("ApplyForce",
			{ {PortDirection::Input,  PortType::Vector2},
			  {PortDirection::Input,  PortType::Bool},
			  {PortDirection::Output, PortType::Int} })
	{

	}

	void ApplyForceMethod::BuildDeclaration(CodeWriter* writer)
	{
		*writer << "public void " << m_Name << "()" << std::endl;
		// Body start
		*writer << CodeManip::BlockBegin;

		BuildExpression(writer);

		// Body end
		*writer << CodeManip::BlockEnd;
	}

	void ApplyForceMethod::BuildExpression(CodeWriter* writer)
	{
		if (!m_InPort.empty())
		{
			if (!m_InPort[0]->GetConnectedPorts().empty())
			{
				auto connectedPort = m_InPort[0]->GetConnectedPorts()[0];
				if (connectedPort)
				{
					*writer << "Physics.ApplyImpulseCenter(ID, ";
					*writer << connectedPort->GetOwner()->BuildLValueName();
					*writer << ", false);" << std::endl;
				}
			}
		}
	}

	std::string ApplyForceMethod::BuildInvoke()
	{
		return "";
	}

}