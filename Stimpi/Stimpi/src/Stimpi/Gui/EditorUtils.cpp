#include "stpch.h"
#include "Stimpi/Gui/EditorUtils.h"

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

}