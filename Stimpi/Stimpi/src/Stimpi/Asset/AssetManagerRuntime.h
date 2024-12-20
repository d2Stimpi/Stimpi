#pragma once

#include "Stimpi/Asset/AssetManagerBase.h"

namespace Stimpi
{
	class AssetManagerRuntime : public AssetManagerBase
	{
	public:
		std::shared_ptr<AssetNew> GetAsset(AssetHandleNew handle) override;
	};
}