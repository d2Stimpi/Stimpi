#pragma once

#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/Scene.h"

#include "ImGui/src/imgui.h"

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

		void SettingsPopupButton(ImVec2 cursorPos, std::string name, std::function<bool()> popupContent);
		void DrawTreeNode(Entity& entity);
		
		// Inspect UI layout parts
		void TagComponentLayout(TagComponent& component);
		void QuadComponentLayout(QuadComponent& component);
		void CircleComponentLayout(CircleComponent& component);
		void ScriptComponentLayout(ScriptComponent& component);
		void SpriteComponentLayout(SpriteComponent& component);
		void SortingGroupComponentLayout(SortingGroupComponent& component);
		void AnimatedSpriteComponentLayout(AnimatedSpriteComponent& component);
		void CameraComponentLayout(CameraComponent& component);
		void RigidBody2DComponentLayout(RigidBody2DComponent& component);
		void BoxCollider2DComponentLayout(BoxCollider2DComponent& component);
		void AddComponentLayout();

		void ShowSelectedEntityComponents(bool show);

		void CreateEntityPopup();
		void SelectedEntityPopup();
	private:
		bool m_Show = true;
		bool m_ShowInspect = true;

		Scene* m_ActiveScene = nullptr;
	};
}