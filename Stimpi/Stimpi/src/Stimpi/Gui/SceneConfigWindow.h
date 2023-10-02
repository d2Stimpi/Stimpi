#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Component.h"

namespace Stimpi
{
	class Entity;

	class SceneConfigWindow
	{
	public:
		SceneConfigWindow();
		~SceneConfigWindow();

		void Draw();

	private:
		void ComponentInspectorWidget();
		
		// Inspect UI layout parts
		void QuadComponentLayout(QuadComponent& component, const std::string& name);
		void TextureComponentLayout(TextureComponent& component, const std::string& name);

	private:
		bool m_Show = true;
		bool m_ShowInspect = true;

		std::function<void(void)> m_InspectFunc{nullptr};
	};
}