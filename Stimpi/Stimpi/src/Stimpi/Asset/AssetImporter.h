#pragma once

#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class AssetImporter
	{
	public:
		static std::shared_ptr<AssetNew> ImportAsset(AssetHandleNew handle, const AssetMetadata& metadata);
	};
}