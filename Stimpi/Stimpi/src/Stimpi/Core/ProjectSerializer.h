#pragma once

#include "Stimpi/Core/Project.h"

namespace Stimpi
{
	class ProjectSerializer
	{
	public:
		static bool Serialize(std::filesystem::path projectFilePath, ProjectConfig project);
		static ProjectConfig Deserialize(std::filesystem::path projectFilePath);
	};
}