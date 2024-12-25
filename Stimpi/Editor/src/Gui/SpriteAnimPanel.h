#pragma once

#include "stpch.h"

#include "Stimpi/Scene/Scene.h"
#include "Stimpi/Graphics/SubTexture.h"
#include "Stimpi/Graphics/Animation/Sprite.h"
#include "Stimpi/Core/Timestep.h"
#include "Stimpi/Scene/Assets/AssetManagerB.h"

namespace Stimpi
{
	class SpriteAnimPanel
	{
	public:
		SpriteAnimPanel();
		~SpriteAnimPanel();

		void SetContext(Scene* scene) { m_Scene = scene; }

		void OnImGuiRender();
		void OnUpdate(Timestep ts);

	private:
		bool m_Show = true;
		bool m_LoopAnim = false;
		uint32_t m_CurrentFrame = 0;
		float m_Duration = 0.0f;

		std::shared_ptr<Sprite> m_Sptire;
		std::shared_ptr<SubTexture> m_SubTextureDisplay;

		Scene* m_Scene;
	};
}