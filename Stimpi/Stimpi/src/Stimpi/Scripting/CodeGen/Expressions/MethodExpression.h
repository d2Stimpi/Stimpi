#pragma once

#include "Stimpi/Core/Core.h"
#include "Expression.h"

// Require in/out port(s) - to check compatibility - virtual methods to handle this
// To be used as based class for Methods

namespace Stimpi
{
	struct ST_API MethodPortParams
	{
		PortDirection m_PortDirection;
		PortType m_PortType;

		MethodPortParams(PortDirection dir, PortType type)
			: m_PortDirection(dir), m_PortType(type)
		{}
	};

	struct ST_API MethodPortLayout
	{
		std::vector<MethodPortParams> m_ParamList;

		MethodPortLayout(std::initializer_list<MethodPortParams> params)
		{
			for (auto item : params)
			{
				m_ParamList.emplace_back(item);
			}
		}

		std::vector<MethodPortParams>::iterator begin() { return m_ParamList.begin(); }
		std::vector<MethodPortParams>::iterator end() { return m_ParamList.end(); }
	};

	class ST_API MethodExpression : public Expression
	{
	public: 
		MethodExpression(const std::string& name, MethodPortLayout layout);

		std::shared_ptr<Port> GetInPort(uint32_t index);
		std::shared_ptr<Port> GetOutPort(uint32_t index);

		// TODO: temp here, move to child class
		void BuildDeclaration(CodeWriter* writer) override;
		void BuildExpression(CodeWriter* writer) override;

		void LinkToInputPort(std::shared_ptr<Port> source, uint32_t targetIndex);
		void LinkToInputPort(std::shared_ptr<Port> source, std::shared_ptr<Port> target);

	protected:
		std::string m_Name;
		// Temp - Ports are specific to each variation of Method Expression (Node)
		std::vector<std::shared_ptr<Port>> m_InPort;
		std::vector<std::shared_ptr<Port>> m_OutPort;

	};
}