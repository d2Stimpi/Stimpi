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
		static ShaderInfo ParseVertexShaderByLine(const std::string& line, ShaderInfo& shaderInfo);
		static bool ParseShaderGraph(Shader* shader, const FilePath& filePath);
	};
}