#pragma once

#include "Stimpi/Asset/AssetManagerBase.h"

namespace Stimpi
{
	class AssetManagerRuntime : public AssetManagerBase
	{
	public:
		std::shared_ptr<AssetNew> GetAsset(AssetHandleNew handle) override;
		bool IsAssetHandleValid(AssetHandleNew handle) override;
		virtual bool IsAssetLoaded(AssetHandleNew handle) override;
	};
}