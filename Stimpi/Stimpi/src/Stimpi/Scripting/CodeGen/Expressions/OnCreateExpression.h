#pragma once

#include "Stimpi/Core/Core.h"
#include "Expression.h"

namespace Stimpi
{

	// Indicates that linked nodes are executed in OnCreate() of the Entity
	class ST_API OnCreateExpression : public Expression
	{
	public:
		OnCreateExpression();

		void BuildDeclaration(CodeWriter* writer) override;
		void BuildExpression(CodeWriter* writer) override;

		std::shared_ptr<Port> GetOutPort() { return m_OutPort; }

	private:
		std::string m_Name;

		std::shared_ptr<Port> m_OutPort;
	};
}