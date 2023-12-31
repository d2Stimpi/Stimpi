#include "stpch.h"
#include "Gui/EditorUtils.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{

	bool EditorUtils::WantCaptureKeyboard()
	{
		ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureKeyboard;
	}

	void EditorUtils::SetActiveItemCaptureKeyboard(bool capture)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsItemActive()) io.WantCaptureKeyboard = capture;
	}

	void EditorUtils::RenderSelection()
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImU32 col = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
		ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionMax().x, pos.y + ImGui::GetTextLineHeight()), col, true);
	}

	glm::vec2 EditorUtils::PositionInCurentWindow(glm::vec2 pos)
	{
		auto res = PositionInCurentWindow(ImVec2(pos.x, pos.y));
		return glm::vec2(res.x, res.y);
	}

	ImVec2 EditorUtils::PositionInCurentWindow(ImVec2 pos)
	{
		ImVec2 winPos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetContentRegionAvail();

		return ImVec2(pos.x + winPos.x, winPos.y + winSize.y - pos.y);
	}

	bool EditorUtils::IsPositionInCurrentWindow(ImVec2 pos)
	{
		ImVec2 winPos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetCurrentWindow()->SizeFull;

		// Y axis is flipped
		ImVec2 min = { winPos.x, winPos.y - winSize.y };
		ImVec2 max = { winPos.x + winSize.x, winPos.y };

		if ((pos.x >= min.x) && (pos.x <= max.x) &&
			(pos.y >= min.y) && (pos.y <= max.y))
		{
			return true;
		}

		return false;
	}

}