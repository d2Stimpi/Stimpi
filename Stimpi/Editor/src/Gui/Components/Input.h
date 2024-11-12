#pragma once

#include <glm/glm.hpp>

namespace Stimpi
{
	namespace UI
	{
		enum class UndoCmdType { VALUE, ENTITY };

		class Input
		{
		public:
			static void SetUndoCmdContext(UndoCmdType type);

			static bool DragFloat3(const char* label, glm::vec3& val);
			static bool DragFloat2(const char* label, glm::vec2& val);
			static bool DragFloat(const char* label, float& val, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		};
	}
}