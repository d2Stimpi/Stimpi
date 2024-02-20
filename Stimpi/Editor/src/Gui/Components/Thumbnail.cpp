#include "stpch.h"
#include "Gui/Components/Thumbnail.h"

#include "Stimpi/Log.h"
#include "Gui/Utils/Utils.h"
#include "ImGui/src/imgui_internal.h""


#define PADDING (10)
#define HALF_PADDING (5)

namespace Stimpi
{
	bool Thumbnail::Button(const char* name, Texture* texture, ImVec2 pos, ImVec2 size)
	{
		bool retVal = false;
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Invisible button to catch input
		ImGui::InvisibleButton(name, size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool hovered = ImGui::IsItemHovered(); // Hovered
		if (hovered)
		{
			retVal = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
		}

		// Draw thumbnail image section
		// 1# Frame spacing and text font size
		float fontSize = ImGui::GetFontSize();
		ImVec2 imgFramePos = { pos.x + HALF_PADDING, pos.y + HALF_PADDING };
		ImVec2 sizeFrame = { size.x - PADDING, size.y - PADDING - fontSize };

		// 2# Thumbnail aspect ratio - avoid stretching of the preview
		float aspect = sizeFrame.x / sizeFrame.y;
		if (aspect > 1) // x > y
		{
			float aspectOffset = (sizeFrame.x - sizeFrame.y) / 2.0f;
			imgFramePos = { imgFramePos.x + aspectOffset, imgFramePos.y };
			sizeFrame = { sizeFrame.y , sizeFrame.y };
		}
		else
		{
			float aspectOffset = (sizeFrame.y - sizeFrame.x) / 2.0f;
			imgFramePos = { imgFramePos.x, imgFramePos.y + aspectOffset };
			sizeFrame = { sizeFrame.x , sizeFrame.x };
		}

		ImVec2 posMin;
		ImVec2 posMax;
		// 3# Maintain texture preview aspect ratio
		ImVec2 textureSize = { (float)texture->GetWidth(), (float)texture->GetHeight() };
		if (textureSize.x > textureSize.y)
		{
			float scaled = textureSize.y / textureSize.x * sizeFrame.y; // scaled texture size
			float offset = (sizeFrame.y - scaled) / 2.0f; // offset to center of image section
			posMin = { imgFramePos.x, imgFramePos.y + offset };
			posMax = { posMin.x + sizeFrame.x, posMin.y + scaled };
		}
		else
		{
			float scaled = textureSize.x / textureSize.y * sizeFrame.x;  // scaled texture size
			float offset = (sizeFrame.x - scaled) / 2.0f; // offset to center of image section
			posMin = { imgFramePos.x + offset, imgFramePos.y };
			posMax = { posMin.x + scaled, posMin.y + sizeFrame.y };
		}
		drawList->AddImage((void*)(intptr_t)texture->GetTextureID(), posMin, posMax, IMVEC2_UV_MIN, IMVEC2_UV_MAX);

		// File name section
		ImVec2 textSize = ImGui::CalcTextSize(name, 0, false, size.x - HALF_PADDING);
		ImVec2 textPos = { pos.x + HALF_PADDING, pos.y + size.y - fontSize - HALF_PADDING };
		// Clip over 2 lines of text
		ImVec4 clipRect = { textPos.x, textPos.y, textPos.x + size.x, textPos.y + 2 * fontSize };
		drawList->AddText(NULL, 0.0f, textPos, IM_COL32(255, 255, 255, 255), name, 0, size.x - HALF_PADDING, &clipRect);


		// Hovered rect
		if (hovered)
		{
			ImVec2 hoverMax = { pos.x + size.x, pos.y + size.y + ( textSize.y - fontSize ) };
			drawList->AddRect(pos, hoverMax, IM_COL32(200, 200, 200, 255), 0.0f, 0, 2.0f);
		}

		return retVal;
	}

}