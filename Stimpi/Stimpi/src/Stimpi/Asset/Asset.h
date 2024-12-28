#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/UUID.h"

namespace Stimpi
{
	using AssetHandle = UUID;

	enum class AssetType
	{
		NONE = 0,
		TEXTURE,
		SHADER
	};

	std::string AssetTypeToString(AssetType type);
	AssetType StringToAssetType(const std::string& str);

	class ST_API Asset
	{
	public:
		AssetHandle m_Handle = 0;

		virtual AssetType GetType() = 0;
	};
}