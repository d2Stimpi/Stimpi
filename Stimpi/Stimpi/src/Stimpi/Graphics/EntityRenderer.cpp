#include "stpch.h"
#include "Stimpi/Graphics/EntityRenderer.h"

#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"

namespace Stimpi
{

	void EntityRenderer::SubmitForRendering(entt::registry& registry)
	{
		auto view = registry.view<DefaultGroupComponent, QuadComponent>();
		view.each(
			[](DefaultGroupComponent& def, QuadComponent& quad) {
				// Note: Possible issue if entity count exceeds 10k or position values are too high which is less likely.
				//	Big number for OrderInLayer may cause precision comparison issues. TODO: use precise float comparison.
				//	Layer count range [1...10].
				//	Consider using a variable value that depends on the entity count.
				def.m_Position = quad.m_Position + 100000.0f * def.m_LayerIndex + 1000000.0f * def.m_OrderInLayer;
			});

		auto axis = Project::GetGraphicsConfig().m_RenderingOrderAxis;
		if (axis == RenderingOrderAxis::X_AXIS)
		{
			registry.sort<DefaultGroupComponent>([&](const auto& lhs, const auto& rhs)
				{
					if (lhs.m_LayerIndex == rhs.m_LayerIndex)
						return lhs.m_Position.x > rhs.m_Position.x;
					return lhs.m_LayerIndex < rhs.m_LayerIndex;
				});
		}
		else if (axis == RenderingOrderAxis::Y_AXIS)
		{
			registry.sort<DefaultGroupComponent>([&](const auto& lhs, const auto& rhs)
				{
					if (lhs.m_LayerIndex == rhs.m_LayerIndex)
						return lhs.m_Position.y > rhs.m_Position.y;
					return lhs.m_LayerIndex < rhs.m_LayerIndex;
				});
		}
		else if (axis == RenderingOrderAxis::Z_AXIS)
		{
			registry.sort<DefaultGroupComponent>([&](const auto& lhs, const auto& rhs)
				{
					if (lhs.m_LayerIndex == rhs.m_LayerIndex)
						return lhs.m_Position.z > rhs.m_Position.z;
					return lhs.m_LayerIndex < rhs.m_LayerIndex;
				});
		}
		/**/
		Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
		auto layeredView = registry.view<DefaultGroupComponent, QuadComponent>();
		layeredView.use<DefaultGroupComponent>();
		layeredView.each([&](auto entity, DefaultGroupComponent& sortGroup, QuadComponent& quad)
			{
				static uint32_t prevLayer = sortGroup.m_LayerIndex;

				if (prevLayer != sortGroup.m_LayerIndex)
				{
					Renderer2D::Instance()->EndScene();
					Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
				}

				if (registry.all_of<SpriteComponent>(entity))
				{
					auto& sprite = registry.get<SpriteComponent>(entity);

					if (sprite.m_Disabled == false)
					{
						if (sprite.m_Enable && sprite.m_TextureAssetHandle)
							Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, sprite, m_DefaultShader.get());
						else
							Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, sprite.m_Color, m_DefaultSolidColorShader.get());
					}
				}
				else if (registry.all_of<AnimatedSpriteComponent>(entity))
				{
					AnimatedSpriteComponent& anim = registry.get<AnimatedSpriteComponent>(entity);
					if (anim.ShouldRender() || m_RuntimeState == RuntimeState::STOPPED)
					{
						if (anim.m_Material == nullptr || !anim.m_UseCustomShader)
							Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, anim, m_DefaultShader.get());
						else
						{
							// For the purpose of CustomShader, texture id is passed to shader
							Renderer2D::Instance()->SubmitCustom(quad.m_Position, quad.m_Size, quad.m_Rotation, anim, anim.m_Material.get(), { entity, this });
						}
					}
				}

				prevLayer = sortGroup.m_LayerIndex;
			});

		// TODO: Move Circle as a part of Renderer component (Circle + Sprite doesn't make much sense)
		auto circleView = registry.view<CircleComponent>();
		if (circleView)
		{
			circleView.each([this](CircleComponent& circle)
				{
					Renderer2D::Instance()->SubmitCircle(circle.m_Position, circle.m_Size, circle.m_Color, circle.m_Thickness, circle.m_Fade);
				});
		}

		Renderer2D::Instance()->EndScene();/**/
	}

}