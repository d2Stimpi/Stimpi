#include "stpch.h"
#include "ClassBuilder.h"
#include "CodeWriter.h"

#include "Expressions/EventExpression.h"
#include "Expressions/OnCreateExpression.h"

#define STR_X(s)	STR(s)
#define STR(s)		#s
#define CLASS_NAME(name) "class " STR_X(name) " : Entity"

// TODO: have a string to write to for each code section - Members, Methods, OnCreate, OnUpdate, etc...

namespace Stimpi
{
	// Used for multiple CodeGraph passes
	enum class CodeGenStates { GEN_VARS, GEN_EVENTS, GEN_METHODS, GEN_STATEMENTS, GEN_FINISH };

	struct CodeGenContext
	{
		CodeGenStates m_GenState = CodeGenStates::GEN_VARS;
		ClassGraph* m_Graph;
		std::string m_OutputFile;
	};

	CodeWriter s_CodeWriter;
	CodeGenContext s_Context;

	void ClassBuilder::GenerateCode(ClassGraph* graph, std::string outputFile)
	{
		if (graph == nullptr)
			return;

		s_Context.m_Graph = graph;
		s_Context.m_OutputFile = outputFile;

		s_CodeWriter.Open(s_Context.m_OutputFile);

		GenerateHeader();

		GenerateMemebers();

		// Methods
		/*for (auto& method : graph->m_ExecutionExpressions)
		{
			s_CodeWriter.Write("\n");
			method->BuildDeclaration(&s_CodeWriter);
		}*/

		// OnCreate()
		GenerateOnCreate();

		// OnUpdate(ts)
		GenerateOnUpdate();

		GenerateEndOfClass();
		// End of file

		s_CodeWriter.Close();
	}

	void ClassBuilder::GenerateHeader()
	{
		// Start of file
		s_CodeWriter << "using System;" << std::endl;
		s_CodeWriter << "using Stimpi;" << std::endl;
		s_CodeWriter << "\nnamespace Sandbox" << std::endl;
		s_CodeWriter << CodeManip::BlockBegin;
		s_CodeWriter << BuildClassName(s_Context.m_Graph->m_ClassName) << std::endl;
		s_CodeWriter << CodeManip::BlockBegin;
	}

	void ClassBuilder::GenerateEndOfClass()
	{
		s_CodeWriter << CodeManip::BlockEnd; // Class
		s_CodeWriter << CodeManip::BlockEnd; // Namespace
	}

	void ClassBuilder::GenerateMemebers()
	{
		// Var declarations
		for (auto& expression : s_Context.m_Graph->m_PassiveExpressions)
		{
			expression->BuildDeclaration(&s_CodeWriter);
		}
	}

	void ClassBuilder::GenerateOnCreate()
	{
		// Init required stuff here
		s_CodeWriter << std::endl;
		s_CodeWriter << "public override void OnCreate()" << std::endl;
		s_CodeWriter << CodeManip::BlockBegin;
		for (auto& expression : s_Context.m_Graph->m_ExecutionExpressions)
		{
			if (expression->GetType() == ExpressionType::Create)
			{
				auto cast = dynamic_cast<OnCreateExpression*>(expression.get());
				auto port = cast->GetOutPort();
				for (auto target : port->GetConnectedPorts())
				{
					auto expr = target->GetOwner();
					expr->BuildExpression(&s_CodeWriter);
				}
			}
		}
		s_CodeWriter << CodeManip::BlockEnd; // OnCreate
	}

	void ClassBuilder::GenerateOnUpdate()
	{
		// - walk the graph to call everything in sequence
		s_CodeWriter << std::endl;
		s_CodeWriter << "public override void OnUpdate(float ts)" << std::endl;
		s_CodeWriter << CodeManip::BlockBegin;
		for (auto& expression : s_Context.m_Graph->m_ExecutionExpressions)
		{
			if (expression->GetType() == ExpressionType::Event)
			{
				expression->BuildExpression(&s_CodeWriter);
				s_CodeWriter << CodeManip::BlockBegin;
				// Find  EvetnExp.OutPort -> connection -> dest Expression
				Expression* ptr = expression.get();
				auto cast = dynamic_cast<EventExpression*>(ptr);
				auto port = cast->GetOutPort();
				for (auto target : port->GetConnectedPorts())
				{
					auto expr = target->GetOwner();
					expr->BuildExpression(&s_CodeWriter);
				}

				s_CodeWriter << CodeManip::BlockEnd; // OnCreate
			}
		}
		s_CodeWriter << CodeManip::BlockEnd; // OnUpdate
	}

	std::string ClassBuilder::BuildClassName(std::string name)
	{
		std::string str = "class ";
		str.append(name).append(" : Entity");
		return str;
	}

	void ClassBuilder::NextState()
	{
		switch (s_Context.m_GenState)
		{
		case CodeGenStates::GEN_VARS: s_Context.m_GenState = CodeGenStates::GEN_EVENTS; break;
		case CodeGenStates::GEN_EVENTS: s_Context.m_GenState = CodeGenStates::GEN_METHODS; break;
		case CodeGenStates::GEN_METHODS: s_Context.m_GenState = CodeGenStates::GEN_STATEMENTS; break;
		case CodeGenStates::GEN_STATEMENTS: s_Context.m_GenState = CodeGenStates::GEN_FINISH; break;
		case CodeGenStates::GEN_FINISH: break;
		}
	}

}