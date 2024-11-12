#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Entity.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	enum class CommandType { NONE, TRANSLATE, SCALE, ROTATE };

	class ST_API Command
	{
	public:
		Command(CommandType type) : m_Type(type) {}

		virtual void Undo() = 0;
		virtual void Redo() = 0;

		CommandType GetType() { return m_Type; }

	private:
		CommandType m_Type;
	};
}