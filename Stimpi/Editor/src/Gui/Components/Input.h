#pragma once

#include <glm/glm.hpp>

namespace Stimpi
{
	namespace UI
	{
		class Input
		{
		public:
			static bool DragFloat3(const char* label, glm::vec3& vec);
		};
	}
}