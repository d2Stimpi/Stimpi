#include "stpch.h"
#include "Stimpi/Graphics/Animation/Animation.h"

#include "Stimpi/Graphics/Animation/AnimationSerializer.h"

namespace Stimpi
{

	Animation::Animation()
	{
	}

	Animation::~Animation()
	{
	}

	AssetHandle Animation::GetTexture()
	{
		return m_TextureHandle;
	}

	std::shared_ptr<Animation> Animation::Create(std::string file)
	{
		std::shared_ptr<Animation> newAnimation = std::make_shared<Animation>();
		AnimationSerializer serializer(newAnimation.get());
		serializer.Deseriealize(file);

		newAnimation->m_AssetPath = file;
		newAnimation->m_Name = newAnimation->m_AssetPath.GetFileName();
		return newAnimation;
	}

}