#pragma once

#include "Gui/Nodes/GraphComponents.h"

namespace Stimpi
{
	class ClassBuilder
	{
	public:
		static void GenerateCode(Graph* graph, std::string outputFile);

	private:
		static void GenerateHeader();
		static void GenerateEndOfClass();

		static void GenerateMemebers();
		static void GenerateOnCreate();
		static void GenerateOnUpdate();

		static std::string BuildClassName(std::string name);
	};
}