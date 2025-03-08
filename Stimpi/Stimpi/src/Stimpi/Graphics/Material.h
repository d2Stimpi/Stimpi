#pragma once

#include "Stimpi/Graphics/Shader.h"

namespace Stimpi
{
	using UniformValuesMap = std::unordered_map<std::string, shader_variant>;

	class ST_API Material
	{
	public:
		Material(AssetHandle shaderHandle);

		std::shared_ptr<Shader> GetShader();
		AssetHandle GetShaderHandle() { return m_ShaderHandle; }
		UniformValuesMap& GetUniformValues() { return m_UnifromValues; }

	private:
		AssetHandle m_ShaderHandle;

		// Uniform data specific to the material
		UniformValuesMap m_UnifromValues;
	};
}