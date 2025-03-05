#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetMetadata.h"
#include "Stimpi/Graphics/Shader.h"

namespace Stimpi
{
	class ST_API ShaderImporter
	{
	public:
		static std::shared_ptr<Asset> ImportShader(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Shader> LoadShader(const FilePath& filePath);

	private:
		static ShaderInfo ParseShaderByLine(const std::string& line, ShaderInfo& shaderInfo);
		static bool LoadShaderGraph(Shader* shader, const FilePath& filePath);
	};
}