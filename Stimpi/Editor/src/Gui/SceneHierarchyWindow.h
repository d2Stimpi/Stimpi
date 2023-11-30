#pragma once

#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/Scene.h"

namespace Stimpi
{
	class Entity;

	class SceneHierarchyWindow
	{
	public:
		SceneHierarchyWindow();
		~SceneHierarchyWindow();

		void OnImGuiRender();

		static void SetPickedEntity(Entity picked);
		static Entity GetSelectedEntity();

	private:
		void ComponentInspectorWidget();
		
		// Inspect UI layout parts
		void TagComponentLayout(TagComponent& component);
		void QuadComponentLayout(QuadComponent& component);
		void ScriptComponentLayout(ScriptComponent& component);
		void SpriteComponentLayout(SpriteComponent& component);
		void CameraComponentLayout(CameraComponent& component);
		void RigidBody2DComponentLayout(RigidBody2DComponent& component);
		void BoxCollider2DComponentLayout(BoxCollider2DComponent& component);
		void AddComponentLayout();

		void ShowSelectedEntityComponents(bool show);
	private:
		bool m_Show = true;
		bool m_ShowInspect = true;

		Scene* m_ActiveScene = nullptr;
	};
}