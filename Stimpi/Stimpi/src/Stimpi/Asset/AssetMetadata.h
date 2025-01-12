#pragma once

#include "Stimpi/Asset/Asset.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Utils/SystemUtils.h"

namespace Stimpi
{
	struct AssetMetadata
	{
		AssetType m_Type = AssetType::NONE;
		FilePath m_FilePath = "";
		// Internal - not serialized
		FileTimeType m_LastWriteTime;
	};
}