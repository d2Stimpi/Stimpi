#include "stpch.h"
#include "UIPayload.h"

#include "Stimpi/Log.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	void UIPayload::BeginSource(const char* type, const void* data, uint32_t size, const char* preview)
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload(type, data, size);
			ImGui::Text("%s", preview);
			ImGui::EndDragDropSource();
		}
	}

	void UIPayload::BeginTarget(const char* type, OnPayloadDropFunction onPalyoadDrop)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type))
			{
				//ST_CORE_INFO("DragDropPayload - Some data dropped");
				onPalyoadDrop(payload->Data, payload->DataSize);
			}
			ImGui::EndDragDropTarget();
		}
	}
}