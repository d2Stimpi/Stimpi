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

	Stimpi::Animation* Animation::Create(std::string file)
	{
		Animation* newAnimation = new Animation();
		AnimationSerializer serializer(newAnimation);
		serializer.Deseriealize(file);

		return newAnimation;
	}

}