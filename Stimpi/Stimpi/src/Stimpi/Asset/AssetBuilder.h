#pragma once

#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class AssetBuilder
	{
	public:
		static std::shared_ptr<Asset> CreateAsset(AssetMetadata& metadata);
	};
}