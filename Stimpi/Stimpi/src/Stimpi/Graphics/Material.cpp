#include "stpch.h"
#include "Stimpi/Graphics/Material.h"

#include "Stimpi/Core/Project.h"
#include "Stimpi/Asset/AssetManager.h"

namespace Stimpi
{

	Material::Material(AssetHandle shaderHandle)
		: m_ShaderHandle(shaderHandle)
	{
		auto shader = GetShader();
		for (auto& uni : shader->GetInfo().m_Uniforms)
		{
			m_UnifromValues[uni.m_Name] = 0.0f;
		}
	}

	std::shared_ptr<Shader> Material::GetShader()
	{
		if (m_ShaderHandle)
		{
			auto shader = AssetManager::GetAsset<Shader>(m_ShaderHandle);
			return shader;
		}

		return nullptr;
	}

}