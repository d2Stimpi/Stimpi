#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Utils/FilePath.h"

namespace Stimpi
{
	using FileTimeType = std::filesystem::file_time_type;

	class ST_API FileSystem
	{
	public:
		static char* ReadBytes(const FilePath& filePath, uint32_t* outSize);
		static FileTimeType LastWriteTime(const FilePath& filePath);
	};

	class ST_API System
	{
	public:
		static std::string ExecuteCmd(const std::string& cmd);
	};
}