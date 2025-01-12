#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/Asset.h"

namespace Stimpi
{
	// TODO: to be removed after AssetImporter update
	struct TextureLoadData
	{
		unsigned char* m_Data = nullptr;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_NumChannels = 0;
		std::string m_FileName;

		TextureLoadData() = default;
	};

	struct TextureSpecification
	{
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_NumChannels = 0;
	};

	class ST_API Texture : public Asset
	{
	public:
		virtual ~Texture();

		virtual void InitEmptyTexture(TextureSpecification spec) = 0;
		virtual void LoadTexture(std::string file) = 0;	// to remove
		virtual void SetData(unsigned char* data) = 0;
		virtual void UseTexture() = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual unsigned int GetTextureID() = 0;

		uint32_t GetWidth() { return m_Spec.m_Width; }
		uint32_t GetHeight() { return m_Spec.m_Height; }

		static std::shared_ptr<Texture> CreateTexture(TextureSpecification spec);
		static Texture* CreateFrameBufferTexture();

		static AssetType GetTypeStatic() { return AssetType::TEXTURE; }
		AssetType GetType() override { return GetTypeStatic(); }

	protected:
		TextureSpecification m_Spec;
	};
}