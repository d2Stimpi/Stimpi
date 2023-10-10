#pragma once

#include "Stimpi/Scene/Component.h"

/**
 * TODO:
 *  - Add component to Entity button
 */

namespace Stimpi
{
	class Entity;

	class SceneHierarchyWindow
	{
	public:
		SceneHierarchyWindow();
		~SceneHierarchyWindow();

		void Draw();

	private:
		void ComponentInspectorWidget();
		
		// Inspect UI layout parts
		void QuadComponentLayout(QuadComponent& component);
		void TextureComponentLayout(TextureComponent& component);
		void AddComponentLayout();

	private:
		bool m_Show = true;
		bool m_ShowInspect = true;

		std::function<void(void)> m_InspectFunc{nullptr};
	};
}