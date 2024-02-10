#pragma once

namespace Stimpi
{
	class EditorConfig
	{
	public:
		EditorConfig();
		~EditorConfig();

		void OnImGuiRender();

	private:
		bool m_Show = true;
	};
}