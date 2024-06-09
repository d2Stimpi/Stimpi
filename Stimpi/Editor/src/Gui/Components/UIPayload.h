#pragma once

#define PAYLOAD_TEXTURE		"Payload_Texture"
#define PAYLOAD_ANIMATION	"Payload_Animation"
#define PAYLOAD_SCENE		"Payload_Scene"

#define PAYLOAD_NODE_VARIABLE_GET	"Payload_Node_Variable_Get"
#define PAYLOAD_NODE_VARIABLE_SET	"Payload_Node_Variable_Set"

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