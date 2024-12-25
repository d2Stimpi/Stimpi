#include "stpch.h"
#include "Stimpi/Asset/TextureImporter.h"

#include "Stimpi/Log.h"
#include "Stimpi/Utils/ThreadPool.h"
#include "Stimpi/Graphics/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Stimpi
{

	std::shared_ptr<Asset> TextureImporter::ImportTexture(AssetHandle handle, const AssetMetadata& metadata)
	{
		// 1. Read image data -> TextureSpecification
		TextureSpecification spec;
		FilePath file = metadata.m_FilePath;

		unsigned char* data = nullptr;
		stbi_set_flip_vertically_on_load(true); // TODO: Move somewhere on init

		data = stbi_load(file.string().c_str(), (int*)&spec.m_Width, (int*)&spec.m_Height, (int*)&spec.m_NumChannels, 0);
		if (!data)
		{
			ST_CORE_ERROR("Failed to load texture: {0}", file.string());
			return nullptr;
		}

		// 2. Generate Texture
		std::shared_ptr<Texture> texture;
		texture.reset(Texture::CreateTexture(file));
		texture->Generate(spec, data);

		// 3. Clear temp data
		stbi_image_free(data);

		return std::static_pointer_cast<Asset>(texture);
	}

}