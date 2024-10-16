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

	void EditorUtils::SetCaptureKeyboard(bool capture)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureKeyboard = capture;
	}

	void EditorUtils::SetActiveItemCaptureKeyboard(bool capture)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsItemActive()) io.WantCaptureKeyboard = capture;
	}

	bool EditorUtils::WantCaptureMouse()
	{
		ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureMouse;
	}

	void EditorUtils::SetCaptureMouse(bool capture)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureMouse = capture;
	}

	void EditorUtils::SetActiveItemCaptureMouse(bool capture)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsItemActive()) io.WantCaptureMouse = capture;
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

	bool EditorUtils::IsMouseContainedInRegion(ImVec2 min, ImVec2 max)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 pos = { io.MousePos.x, io.MousePos.y };

		if ((pos.x >= min.x) && (pos.x <= max.x) &&
			(pos.y >= min.y) && (pos.y <= max.y))
		{
			return true;
		}

		return false;
	}

	std::string EditorUtils::StringTrimByWidth(std::string text, float widthLimit)
	{
		float lenSize = text.length() * (ImGui::GetFontSize() / 2.0f + 1); // approximation "good enough"
		if (lenSize > widthLimit)
		{
			uint32_t len = widthLimit / (ImGui::GetFontSize() / 2.0f + 1);
			text = text.substr(0, len);
			text.append("...");
		}
		return text;
	}

	bool EditorUtils::IsWindowActiveAndVisible(const char* name)
	{
		ImGuiWindow* window = ImGui::FindWindowByName(name);
		return (window->Active) && (!window->Hidden);
	}

}