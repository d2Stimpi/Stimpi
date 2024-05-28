#pragma once

#include "Stimpi/Core/Core.h"
#include "Expression.h"

namespace Stimpi
{
	class ST_API EventExpression : public Expression
	{
	public:
		EventExpression(std::string name);

		void BuildDeclaration(CodeWriter* writer) override;
		void BuildExpression(CodeWriter* writer) override;

		std::shared_ptr<Port> GetOutPort() { return m_OutPort; }

	private:
		std::string m_Name;

		std::shared_ptr<Port> m_OutPort;
	};
}