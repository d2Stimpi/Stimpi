#include "stpch.h"
#include "Stimpi/Asset/Asset.h"

namespace Stimpi
{

	std::string AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case Stimpi::AssetType::NONE:		return std::string("NONE");
		case Stimpi::AssetType::TEXTURE:	return std::string("TEXTURE");
		case Stimpi::AssetType::SHADER:		return std::string("SHADER");
		default: return std::string("NONE");
		}
	}

}