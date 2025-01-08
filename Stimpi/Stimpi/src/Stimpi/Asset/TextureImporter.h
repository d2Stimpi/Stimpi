#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetMetadata.h"
#include "Stimpi/Graphics/Texture.h"

namespace Stimpi
{
	class ST_API TextureImporter
	{
	public:
		static std::shared_ptr<Asset> ImportTexture(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Texture> LoadTexture(const FilePath& filePath);
	};
}