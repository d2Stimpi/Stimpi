#include "stpch.h"
#include "Stimpi/Asset/AssetManagerRuntime.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	std::shared_ptr<AssetNew> AssetManagerRuntime::GetAsset(AssetHandleNew handle)
	{
		ST_CORE_WARN("AssetManagerRuntime::GetAsset - not implemented!");
		return nullptr;
	}

	bool AssetManagerRuntime::IsAssetHandleValid(AssetHandleNew handle)
	{
		ST_CORE_WARN("AssetManagerRuntime::IsHandleValid - not implemented!");
		return false;
	}

	bool AssetManagerRuntime::IsAssetLoaded(AssetHandleNew handle)
	{
		ST_CORE_WARN("AssetManagerRuntime::IsAssetLoaded - not implemented!");
		return false;
	}

}