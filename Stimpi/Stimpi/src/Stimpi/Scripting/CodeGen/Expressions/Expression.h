#pragma once

#include "Stimpi/Core/Core.h"
#include "../CodeWriter.h"

#include <string>
#include <vector>
#include <memory>

#define EXPRESSION_GET_STATIC_TYPE(type)	static ExpressionType GetStaticType() { return type; }

namespace Stimpi
{
	enum class ExpressionType {None = 0, Variable, Property, Method, Statement, Event, Create};

	class ST_API Expression
	{
	public:
		Expression(ExpressionType type);
		~Expression();

		// Common - Passive/Execution
		virtual void BuildDeclaration(CodeWriter* writer) = 0;
		virtual void BuildExpression(CodeWriter* writer) = 0;
		// Passive
		virtual std::string BuildLValueName() { return ""; }
		// Execution
		virtual std::string BuildInvoke() { return ""; }

		ExpressionType GetType() { return m_Type; }
		void SetVisited(bool visited) { m_Visited = visited; }

	private:
		ExpressionType m_Type;
		bool m_Visited = false;
	};

	enum class PortType { None = 0, Bool, Int, Vector2 };
	enum class PortDirection { Input = 0, Output };
	
	std::string PortTypeToString(PortType portType);

	class ST_API Port
	{
	public:
		Port(PortType type);

		void SetOwner(Expression* expression) { m_Owner.reset(expression); }
		std::shared_ptr<Expression> GetOwner() { return m_Owner; }

		void Connect(std::shared_ptr<Port> source);
		bool HasConnection() { return !m_ConnectedPorts.empty(); }
		std::vector<std::shared_ptr<Port>>& GetConnectedPorts() { return m_ConnectedPorts; }

		PortType& GetType() { return m_Type; }

	private:
		uint32_t m_PortID; // TODO;
		PortType m_Type = PortType::None;
		std::shared_ptr<Expression> m_Owner;

		std::vector<std::shared_ptr<Port>> m_ConnectedPorts;
	};
}