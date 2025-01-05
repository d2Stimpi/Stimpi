#include "stpch.h"
#include "Stimpi/Asset/TextureImporter.h"

#include "Stimpi/Log.h"
#include "Stimpi/Utils/ThreadPool.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Core/Project.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Stimpi
{

	std::shared_ptr<Asset> TextureImporter::ImportTexture(AssetHandle handle, const AssetMetadata& metadata)
	{
		FilePath assetPath = Project::GetAssestsDir() / metadata.m_FilePath.string();
		return LoadTexture(assetPath);
	}

	std::shared_ptr<Stimpi::Asset> TextureImporter::LoadTexture(const FilePath& filePath)
	{
		// 1. Read image data -> TextureSpecification
		TextureSpecification spec;

		unsigned char* data = nullptr;
		stbi_set_flip_vertically_on_load(true); // TODO: Move somewhere on init

		data = stbi_load(filePath.string().c_str(), (int*)&spec.m_Width, (int*)&spec.m_Height, (int*)&spec.m_NumChannels, 0);
		if (!data)
		{
			ST_CORE_ERROR("Failed to load texture: {0}", filePath.string());
			return nullptr;
		}

		// 2. Generate Texture
		std::shared_ptr<Texture> texture = Texture::CreateTexture(spec);
		texture->SetData(data);

		// 3. Clear temp data
		stbi_image_free(data);

		return texture;
	}

}