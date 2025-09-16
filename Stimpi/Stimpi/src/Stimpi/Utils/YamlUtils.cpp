#include "stpch.h"
#include "Stimpi/Utils/YamlUtils.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	bool YamlUtils::NodesEqual(const YAML::Node& nodeA, const YAML::Node& nodeB, const std::vector<std::string> ignoreKeys)
	{
		if (nodeA.IsNull() && nodeB.IsNull())
			return true;

		if (nodeA.IsNull() != nodeB.IsNull())
			return false;

		if (nodeA.Type() != nodeB.Type())
			return false;

		switch (nodeA.Type())
		{
		case YAML::NodeType::Scalar:
			//ST_CORE_INFO("A: {}, B: {}", nodeA.as<std::string>(), nodeB.as<std::string>());
			return nodeA.as<std::string>() == nodeB.as<std::string>();
		case YAML::NodeType::Sequence:
			if (nodeA.size() != nodeB.size())
				return false;
			
			for (size_t i = 0; i < nodeA.size(); i++)
			{
				if (!NodesEqual(nodeA[i], nodeB[i], ignoreKeys))
					return false;
			}
			return true;
		case YAML::NodeType::Map:
		{
			if (nodeA.size() != nodeB.size())
				return false;

			YAML::const_iterator itA = nodeA.begin();
			YAML::const_iterator itB = nodeB.begin();
			for (size_t i = 0; i < nodeA.size(); i++)
			{
				const std::string keyA = itA->first.as<std::string>();
				const std::string keyB = itB->first.as<std::string>();

				if (keyA != keyB)
					return false;

				if (std::find(ignoreKeys.begin(), ignoreKeys.end(), keyA) == ignoreKeys.end())
				{
					if (!NodesEqual(itA->second, itB->second, ignoreKeys))
						return false;
				}

				itA++;
				itB++;
			}
			return true;
		}
		default:
			ST_CORE_INFO("[Yaml node compare] Unhandled node type {}", (int)nodeA.Type());
			break;
		}

		return false;
	}

}