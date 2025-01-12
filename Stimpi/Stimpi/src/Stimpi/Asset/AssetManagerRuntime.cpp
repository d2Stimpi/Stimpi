#include "stpch.h"
#include "Stimpi/Asset/AssetManagerRuntime.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	std::shared_ptr<Asset> AssetManagerRuntime::GetAsset(AssetHandle handle)
	{
		ST_CORE_WARN("AssetManagerRuntime::GetAsset - not implemented!");
		return nullptr;
	}

	bool AssetManagerRuntime::IsAssetHandleValid(AssetHandle handle)
	{
		ST_CORE_WARN("AssetManagerRuntime::IsHandleValid - not implemented!");
		return false;
	}

	bool AssetManagerRuntime::IsAssetLoaded(AssetHandle handle)
	{
		ST_CORE_WARN("AssetManagerRuntime::IsAssetLoaded - not implemented!");
		return false;
	}

}