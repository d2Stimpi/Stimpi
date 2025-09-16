#pragma once

#include "Stimpi/Core/Core.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class ST_API YamlUtils
	{
	public:
		static bool NodesEqual(const YAML::Node& nodeA, const YAML::Node& nodeB, const std::vector<std::string> ignoreKeys = {});
	};
}