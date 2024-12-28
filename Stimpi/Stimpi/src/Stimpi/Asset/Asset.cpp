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

	Stimpi::AssetType StringToAssetType(const std::string& str)
	{
		if (str == "NONE") return AssetType::NONE;
		else if (str == "TEXTURE") return AssetType::TEXTURE;
		else if (str == "SHADER") return AssetType::SHADER;
		else return AssetType::NONE;
	}

}