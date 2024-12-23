#pragma once

#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class TextureImporter
	{
	public:
		static std::shared_ptr<AssetNew> ImportTexture(AssetHandleNew handle, const AssetMetadata& metadata);
	};
}