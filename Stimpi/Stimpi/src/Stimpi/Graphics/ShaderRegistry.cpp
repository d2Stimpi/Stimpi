#include "stpch.h"
#include "Stimpi/Graphics/ShaderRegistry.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Project.h"

namespace Stimpi
{

	std::unordered_map<std::string, AssetHandle> s_ShaderRegistry;
	std::vector<std::string> s_ShaderNames;	// Cached list of registered shaders

	void ShaderRegistry::RegisterShader(AssetHandle shaderHandle)
	{
		AssetMetadata metadata = Project::GetEditorAssetManager()->GetAssetMetadata(shaderHandle);
		if (metadata.m_Type != AssetType::NONE)	// equivalent to IsAssetHandleValid()
		{
			std::string shaderName = metadata.m_FilePath.GetFileName();

			auto find = s_ShaderRegistry.find(shaderName);
			if (find == s_ShaderRegistry.end())
			{
				s_ShaderRegistry[shaderName] = shaderHandle;
				s_ShaderNames.push_back(shaderName);
			}
		}
	}

	void ShaderRegistry::UnregisterShader(AssetHandle shaderHandle)
	{
		AssetMetadata metadata = Project::GetEditorAssetManager()->GetAssetMetadata(shaderHandle);
		if (metadata.m_Type != AssetType::NONE)	// equivalent to IsAssetHandleValid()
		{
			std::string shaderName = metadata.m_FilePath.GetFileName();

			auto find = s_ShaderRegistry.find(shaderName);
			if (find != s_ShaderRegistry.end())
			{
				s_ShaderRegistry.erase(shaderName);
				
				// Find and remove shader name from list
				auto findName = std::find(s_ShaderNames.begin(), s_ShaderNames.end(), shaderName);
				if (findName != s_ShaderNames.end())
					s_ShaderNames.erase(findName);
			}
		}
	}

	std::vector<std::string>& ShaderRegistry::GetShaderNames()
	{
		return s_ShaderNames;
	}

	Stimpi::AssetHandle ShaderRegistry::GetShaderHandle(const std::string& shaderName)
	{
		return s_ShaderRegistry[shaderName];
	}

}