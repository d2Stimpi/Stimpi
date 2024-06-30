#pragma once

#include "ImGui/src/imgui.h"
#include <glm/glm.hpp>

namespace Stimpi
{
	class EditorUtils
	{
	public:
		// Keyboard capture helpers
		static bool WantCaptureKeyboard();
		static void SetActiveItemCaptureKeyboard(bool capture);

		// Selection render helper
		static void RenderSelection();

		// Window position calculations
		static glm::vec2 PositionInCurentWindow(glm::vec2 pos);
		static ImVec2 PositionInCurentWindow(ImVec2 pos);

		// Bound checks
		static bool IsPositionInCurrentWindow(ImVec2 pos);
		static bool IsMouseContainedInRegion(ImVec2 min, ImVec2 max);

		// Text trim by size limit
		static std::string StringTrimByWidth(std::string text, float widthLimit);
	};
}