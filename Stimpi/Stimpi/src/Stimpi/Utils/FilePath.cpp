#include "stpch.h"
#include "Stimpi/Utils/FilePath.h"

namespace Stimpi
{

	FilePath::FilePath(std::filesystem::path dir, std::string name)
	{
		m_FileDir = dir;
		m_FileName = name;
		m_FilePath = dir / std::filesystem::path(name);
	}

	FilePath::FilePath(std::filesystem::path fullPath)
	{
		m_FilePath = fullPath;
		m_FileName = fullPath.filename().string();
		m_FileDir = fullPath.parent_path();
	}

	FilePath::FilePath(std::string fullPath)
		: FilePath(std::filesystem::path(fullPath))
	{
	}

	FilePath::FilePath(const char* fullPath)
		: FilePath(std::string(fullPath))
	{
	}

}