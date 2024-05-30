#include "stpch.h"
#include "ClassBuilder.h"

#include "Stimpi/Core/Core.h"

#include "Gui/CodeGen/CodeWriter.h"
#include "Gui/CodeGen/CodeGenerators.h"
#include "Gui/Nodes/GraphComponents.h"

namespace Stimpi
{
	struct CodeGenContext
	{
		//CodeGenStates m_GenState = CodeGenStates::GEN_VARS;
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
		for (auto& node : s_Context.m_Graph->m_Nodes)
		{
			if (node->m_Type == Node::NodeType::Variable)
			{
				auto outPin = node->m_OutPins[0];
				s_CodeWriter << CodeGenerators::GetValueName(node.get()) << " " << outPin->m_Text << " = $val;" << std::endl;
			}
		}
	}

	void ClassBuilder::GenerateOnCreate()
	{
		s_CodeWriter << std::endl;
		s_CodeWriter << "public override void OnCreate()" << std::endl;
		s_CodeWriter << CodeManip::BlockBegin;

		// TODO: gen code here

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