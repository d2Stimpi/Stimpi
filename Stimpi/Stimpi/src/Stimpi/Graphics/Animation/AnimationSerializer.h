#pragma once

#include "Stimpi/Graphics/Animation/Animation.h"

namespace Stimpi
{
	class AnimationSerializer
	{
	public:
		AnimationSerializer(Animation* animation);

		void Serialize(const std::string& filePath);
		bool Deseriealize(const std::string& filePath);

	private:
		Animation* m_Animation;
	};
}