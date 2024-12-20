#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/UUID.h"

namespace Stimpi
{
	using AssetHandleNew = UUID;

	enum class AssetTypeNew
	{
		NONE = 0,
		TEXTURE,
		SHADER
	};

	class ST_API AssetNew
	{
	public:
		AssetHandleNew m_Handle = 0;

		virtual AssetTypeNew GetType() { return AssetTypeNew::NONE; };
	};
}