#pragma once

#include "Stimpi/Core/Core.h"
#include "ClassGraph.h"

namespace Stimpi
{
	class ST_API ClassBuilder
	{
	public:
		static void GenerateCode(ClassGraph* graph, std::string outputFile);

	private:
		static void GenerateHeader();
		static void GenerateEndOfClass();

		static void GenerateMemebers();
		static void GenerateOnCreate();
		static void GenerateOnUpdate();

		static std::string BuildClassName(std::string name);

		static void NextState();

	private:
	};
}