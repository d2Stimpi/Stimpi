#pragma once

#include "Stimpi/Asset/AssetManagerBase.h"

namespace Stimpi
{
	using AssetRegistry = std::unordered_map<UUID, std::shared_ptr<AssetNew>>;

	class AssetManagerEditor : public AssetManagerBase
	{
	public:
		std::shared_ptr<AssetNew> GetAsset(AssetHandleNew handle) override;

	private:
		AssetRegistry m_LoadedAssets;
	};
}