#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Serializer.h"

namespace Stimpi
{
	class ST_API ResourceManager
	{
	public:
		static ResourceManager* Instance();

		void ClearFile(const std::string& fileName);
		void WriteToFile(const std::string& fileName, const char* data);
		void WriteToFileAppend(const std::string& fileName, const char* data);
		void WriteToFileAppend(const std::string& fileName, const YAML::Node& data);

		// TODO: tmp test
		void Test();
	};
}