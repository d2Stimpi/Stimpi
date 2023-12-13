#pragma once

namespace Stimpi
{
	class SearchPopup
	{
	public:
		static bool OnImGuiRender(const char* name, bool* show, std::vector<std::string> &data);
		static std::string GetSelection();
	};
}