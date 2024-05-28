#pragma once

#include "Stimpi/Core/Core.h"
#include "Expression.h"

namespace Stimpi
{
	class ST_API VariableExpression : public Expression
	{
	public:
		VariableExpression(const std::string& name, PortType type);
		~VariableExpression();

		void BuildDeclaration(CodeWriter* writer) override;
		void BuildExpression(CodeWriter* writer) override;
		std::string BuildLValueName() override;
		
		std::shared_ptr<Port> GetOutPort() { return m_Port; }

		EXPRESSION_GET_STATIC_TYPE(ExpressionType::Variable)
	private:
		std::string m_Name;
		std::shared_ptr<Port> m_Port;
	};
}