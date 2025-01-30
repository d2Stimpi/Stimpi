#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/Asset.h"

namespace Stimpi
{
	class ST_API ShaderRegistry
	{
	public:
		static void RegisterShader(AssetHandle shaderHandle);
		static void UnregisterShader(AssetHandle shaderHandle);

		static std::vector<std::string>& GetShaderNames();
		static AssetHandle GetShaderHandle(const std::string& shaderName);
	};
}