#pragma once

#include "Stimpi/Asset/AssetManagerBase.h"

namespace Stimpi
{
	class AssetManagerNew
	{
	public:
		template<typename T>
		static std::shared_ptr<T> GetAsset(AssetHandleNew handle)
		{
			std::shared_ptr<AssetNew> asset = Project::GetAssetManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}
	};
}