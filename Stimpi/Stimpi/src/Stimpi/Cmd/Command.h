#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Entity.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	class ST_API Command
	{
	public:
		Command() = default;

		virtual void Undo() = 0;
		virtual void Redo() = 0;

	private:
	};
}