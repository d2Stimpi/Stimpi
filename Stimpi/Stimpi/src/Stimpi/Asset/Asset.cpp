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
		case Stimpi::AssetType::SCENE:		return std::string("SCENE");
		case Stimpi::AssetType::ANIMATION:	return std::string("ANIMATION");
		case Stimpi::AssetType::PREFAB:		return std::string("PREFAB");
		default: return std::string("NONE");
		}
	}

	AssetType StringToAssetType(const std::string& str)
	{
		if (str == "NONE")				return AssetType::NONE;
		else if (str == "TEXTURE")		return AssetType::TEXTURE;
		else if (str == "SHADER")		return AssetType::SHADER;
		else if (str == "SCENE")		return AssetType::SCENE;
		else if (str == "ANIMATION")	return AssetType::ANIMATION;
		else if (str == "PREFAB")		return AssetType::PREFAB;
		else return AssetType::NONE;
	}

	AssetType AssetUtils::GetAssetType(FilePath filePath)
	{
		if (std::filesystem::is_directory(filePath))
			return AssetType::NONE;

		std::string extension = filePath.GetFileExtension();
		if (extension == ".png" || extension == ".PNG" || extension == ".jpg" || extension == ".JPG")
			return AssetType::TEXTURE;
		if (extension == ".shader")
			return AssetType::SHADER;
		if (extension == ".d2s")
			return AssetType::SCENE;
		if (extension == ".anim")
			return AssetType::ANIMATION;
		if (extension == ".fab")
			return AssetType::PREFAB;

		return AssetType::NONE;
	}

}