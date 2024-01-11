#pragma once

#include "Stimpi/Core/Core.h"
#include <string>

namespace Stimpi
{
	class ST_API FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);

		static std::string OpenFolder();
	};
}