#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	class ST_API FileSystem
	{
	public:
		static char* ReadBytes(const std::string& filepath, uint32_t* outSize);
	};

	class ST_API System
	{
	public:
		static std::string ExecuteCmd(const std::string& cmd);
	};
}