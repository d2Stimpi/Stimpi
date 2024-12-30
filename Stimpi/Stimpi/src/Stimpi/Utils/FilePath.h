#pragma once

#include "Stimpi/Core/Core.h"

#include <filesystem>

namespace Stimpi
{
	class ST_API FilePath
	{
	public:
		FilePath() = default;
		FilePath(std::filesystem::path dir, std::string name);
		FilePath(std::filesystem::path fullPath);
		FilePath(std::string fullPath);
		FilePath(const char* fullPath);

		std::filesystem::path& GetDir() { return m_FileDir; }
		std::filesystem::path& GetPath() { return m_FilePath; }
		std::filesystem::path GetPath() const { return m_FilePath; }
		std::string& GetFileName() { return m_FileName; }
		std::string GetFileExtension() { return m_FilePath.extension().string(); }
		std::string GetFileNameStem() { return m_FilePath.stem().string(); }

		std::filesystem::path GetRelativePath(const std::filesystem::path& basePath);

		std::string string() { return m_FilePath.string(); }
		std::string string() const { return m_FilePath.string(); }

		bool Exists() { return std::filesystem::exists(m_FilePath); }

		operator std::string() { return m_FilePath.string(); }
		operator std::filesystem::path() { return m_FilePath; }
		operator bool() { return Exists(); }

	private:
		std::filesystem::path m_FilePath;
		std::filesystem::path m_FileDir;
		std::string m_FileName;
	};
}