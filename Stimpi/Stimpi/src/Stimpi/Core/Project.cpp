#include "stpch.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Core/ProjectSerializer.h"


namespace Stimpi
{
	ProjectConfig Project::m_ActiveProject;

	void Project::Save(std::filesystem::path projectFilePath)
	{
		ProjectSerializer::Serialize(projectFilePath, m_ActiveProject);
	}

	void Project::Load(std::filesystem::path projectFilePath)
	{
		m_ActiveProject = ProjectSerializer::Deserialize(projectFilePath);
	}
}