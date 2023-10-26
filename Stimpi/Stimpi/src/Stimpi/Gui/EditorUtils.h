#pragma once

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
	};
}