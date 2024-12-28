#pragma once

#include "Stimpi/Asset/Asset.h"
#include "Stimpi/Utils/FilePath.h"

namespace Stimpi
{
	struct AssetMetadata
	{
		AssetType m_Type = AssetType::NONE;
		FilePath m_FilePath = "";
	};
}