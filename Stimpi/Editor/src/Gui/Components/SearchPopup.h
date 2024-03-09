#pragma once

namespace Stimpi
{
	class SearchPopup
	{
	public:
		static bool OnImGuiRender(std::vector<std::string>& data);
		static std::string GetSelection();
		static void OpenPopup();
		static void ClosePopup();
	};
}