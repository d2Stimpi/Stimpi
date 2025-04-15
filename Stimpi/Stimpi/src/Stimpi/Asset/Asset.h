#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/UUID.h"
#include "Stimpi/Utils/FilePath.h"

namespace Stimpi
{
	using AssetHandle = UUID;

	enum class ST_API AssetType
	{
		NONE = 0,
		TEXTURE,
		SHADER,
		SCENE,
		ANIMATION
	};

	std::string AssetTypeToString(AssetType type);
	AssetType StringToAssetType(const std::string& str);

	class ST_API AssetUtils
	{
	public:
		static AssetType GetAssetType(FilePath filePath);
	};


	class ST_API Asset
	{
	public:
		AssetHandle m_Handle = 0;	// TODO: should be used? Maybe for easier metadata lookup?

		virtual AssetType GetType() = 0;

		operator bool() const { return m_Handle != 0; }
	};
}