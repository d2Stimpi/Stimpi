#pragma once

#include "Stimpi/Asset/AssetManagerBase.h"

namespace Stimpi
{
	class AssetManager
	{
	public:
		template<typename T>
		static std::shared_ptr<T> GetAsset(AssetHandle handle)
		{
			std::shared_ptr<Asset> asset = Project::GetAssetManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static AssetHandle GetAssetHandle(const AssetMetadata& metadata);
	};
}