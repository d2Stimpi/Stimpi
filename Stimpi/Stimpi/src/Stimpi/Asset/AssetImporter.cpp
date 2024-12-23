#include "stpch.h"
#include "Stimpi/Asset/AssetImporter.h"

#include "Stimpi/Asset/TextureImporter.h"

namespace Stimpi
{
	using AssetImportFunction = std::function<std::shared_ptr<AssetNew>(AssetHandleNew, const AssetMetadata&)>;

	std::unordered_map<AssetTypeNew, AssetImportFunction> s_AssetImporters =
	{
		{ AssetTypeNew::SHADER, nullptr },
		{ AssetTypeNew::TEXTURE, TextureImporter::ImportTexture }
	};

	std::shared_ptr<AssetNew> AssetImporter::ImportAsset(AssetHandleNew handle, const AssetMetadata& metadata)
	{
		if (s_AssetImporters.find(metadata.m_Type) == s_AssetImporters.end())
		{
			ST_CORE_ERROR("AssetImporter::ImportAsset: Unknown asset type!");
			return nullptr;
		}

		return s_AssetImporters.at(metadata.m_Type)(handle, metadata);
	}

}