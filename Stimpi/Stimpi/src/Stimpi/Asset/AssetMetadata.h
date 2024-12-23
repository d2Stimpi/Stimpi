#pragma once

#include "Stimpi/Asset/AssetNew.h"
#include "Stimpi/Utils/FilePath.h"

namespace Stimpi
{
	struct AssetMetadata
	{
		AssetTypeNew m_Type;
		FilePath m_FilePath;
	};
}