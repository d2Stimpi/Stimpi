#pragma once

#define PAYLOAD_TEXTURE		"Payload_Texture"
#define PAYLOAD_ANIMATION	"Payload_Animation"
#define PAYLOAD_SCENE		"Payload_Scene"

namespace Stimpi
{
	using OnPayloadDropFunction = std::function<void(void* data, uint32_t size)>;

	class UIPayload
	{
	public:

		static void BeginSource(const char* type, const void* data, uint32_t size, const char* preview = "");
		static void BeginTarget(const char* type, OnPayloadDropFunction onPalyoadDrop);
	};
}