#pragma once

#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class TextureImporter
	{
	public:
		static std::shared_ptr<Asset> ImportTexture(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Asset> LoadTexture(const FilePath& filePath);
	};
}