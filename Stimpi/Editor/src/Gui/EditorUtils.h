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
	};
}