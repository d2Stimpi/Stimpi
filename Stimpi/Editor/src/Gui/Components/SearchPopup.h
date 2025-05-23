#pragma once

namespace Stimpi
{
	/**
	 * Special data string starting with "#" indicates separator with text
	 */

	class SearchPopup
	{
	public:
		static bool OnImGuiRender(const char* popupName, std::vector<std::string>& data);
		static std::string GetSelection();
		static void OpenPopup(const char* popupName);
		static void ClosePopup();
		static bool IsActive();
	};
}