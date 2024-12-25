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
		virtual void Generate(TextureSpecification spec, unsigned char* data) = 0;
		virtual void UseTexture() = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual unsigned int GetTextureID() = 0;

		// Async loading
		virtual void LoadDataAsync(std::string file) = 0; // to remove

		uint32_t GetWidth() { return m_Spec.m_Width; }
		uint32_t GetHeight() { return m_Spec.m_Height; }
		std::string& GetAssetPath() { return m_AssetPath; }

		static Texture* CreateTexture(std::string filePath);
		static Texture* CreateFrameBufferTexture();

		static AssetType GetTypeStatic() { return AssetType::TEXTURE; }
		AssetType GetType() override { return GetTypeStatic(); }

		// AssetManager
		static Texture* Create(std::string file) { return CreateTexture(file); }	// to remove
		virtual bool Loaded() = 0;	// to remove
	protected:
		TextureSpecification m_Spec;
		std::string m_AssetPath;
	};
}