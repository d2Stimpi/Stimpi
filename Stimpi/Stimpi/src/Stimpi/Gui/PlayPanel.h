#pragma once

#include "Stimpi/Scene/Scene.h"

namespace Stimpi
{
	class PlayPanel
	{
	public:
		PlayPanel();
		~PlayPanel();

		void OnImGuiRender();

	private:
		bool m_Show = true;
		Scene* m_ActiveScene = nullptr;
	};
}