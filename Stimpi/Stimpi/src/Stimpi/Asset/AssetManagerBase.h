#pragma once

#include "Stimpi/Asset/AssetNew.h"

namespace Stimpi
{
	class AssetManagerBase
	{
	public:
		virtual std::shared_ptr<AssetNew> GetAsset(AssetHandleNew handle) = 0;
	};
}