#include "stpch.h"
#include "ClassBuilder.h"

#include "Stimpi/Core/Core.h"

#include "Gui/CodeGen/CodeWriter.h"
#include "Gui/Nodes/GraphComponents.h"

#include "Gui/Nodes/Nodes.h"

namespace Stimpi
{
	struct CodeGenContext
	{
		Graph* m_Graph;
		std::string m_OutputFile;
	};

	CodeWriter s_CodeWriter;
	CodeGenContext s_Context;

	void ClassBuilder::GenerateCode(Graph* graph, std::string outputFile)
	{
		if (graph == nullptr)
			return;

		s_Context.m_Graph = graph;
		s_Context.m_OutputFile = outputFile;

		s_CodeWriter.Open(s_Context.m_OutputFile);

		GenerateHeader();

		GenerateMemebers();

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
		s_CodeWriter << BuildClassName(s_Context.m_Graph->m_Name) << std::endl;
		s_CodeWriter << CodeManip::BlockBegin;
	}

	void ClassBuilder::GenerateEndOfClass()
	{
		s_CodeWriter << CodeManip::BlockEnd; // Class
		s_CodeWriter << CodeManip::BlockEnd; // Namespace
	}

	void ClassBuilder::GenerateMemebers()
	{
		for (auto& var : s_Context.m_Graph->m_Variables)
		{
			s_CodeWriter << VariableValueTypeToString(var->m_ValueType) << " " << var->m_Text << " = " << var->m_Value << ";" << std::endl;
		}
	}

	void ClassBuilder::GenerateOnCreate()
	{
		s_CodeWriter << std::endl;
		s_CodeWriter << "public override void OnCreate()" << std::endl;
		s_CodeWriter << CodeManip::BlockBegin;

		// Follow execution flow starting from "root" node
		auto onCreateNode = s_Context.m_Graph->FindNodeByName(OnCreateNode::GetName());
		if (onCreateNode)
		{
			// TODO: split Pin to FlowPin DataPin: onCreateNode->m_DataPin[0]...
			// m_OutPins[0] - data out pin from VariableSetNode
			auto next = onCreateNode->m_OutPins[0]->m_ConnectedPins[0]->m_ParentNode;
			next->m_CodeComponent->GenerateExpression(next, &s_CodeWriter);
		}

		s_CodeWriter << CodeManip::BlockEnd;
	}

	void ClassBuilder::GenerateOnUpdate()
	{
		s_CodeWriter << std::endl;
		s_CodeWriter << "public override void OnUpdate(float ts)" << std::endl;
		s_CodeWriter << CodeManip::BlockBegin;

		// TODO: gen code here

		s_CodeWriter << CodeManip::BlockEnd;
	}

	std::string ClassBuilder::BuildClassName(std::string name)
	{
		std::string str = "class ";
		str.append(name).append(" : Entity");
		return str;
	}

}