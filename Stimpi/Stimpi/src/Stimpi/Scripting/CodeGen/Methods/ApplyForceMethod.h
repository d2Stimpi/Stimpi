#pragma once

#include "Stimpi/Core/Core.h"
#include "../Expressions/MethodExpression.h"

namespace Stimpi
{
	class ST_API ApplyForceMethod : public MethodExpression
	{
	public:
		ApplyForceMethod();

		void BuildDeclaration(CodeWriter* writer) override;
		void BuildExpression(CodeWriter* writer) override;
		std::string BuildInvoke() override;
	private:

	};
}