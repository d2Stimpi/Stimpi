#include "stpch.h"
#include "Stimpi/Utils/SystemUtils.h"

#include "Stimpi/Log.h"

#include <cstdlib>

namespace Stimpi
{
	char* FileSystem::ReadBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			ST_CORE_ERROR("Failed to open {}", filepath);
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
		{
			ST_CORE_ERROR("Failed to read {} - file is empty", filepath);
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	std::string System::ExecuteCmd(const std::string& cmd)
	{
		std::system(cmd.c_str());
		return "";
	}
}