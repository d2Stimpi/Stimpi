#pragma once

#include "Stimpi/Scene/Entity.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	enum class CommandType { TRANSLATE, SCALE, ROTATE };

	class Command
	{
	public:
		Command(CommandType type) : m_Type(type) {}

		virtual void Execute() = 0;

		CommandType GetType() { return m_Type; }

	private:
		CommandType m_Type;
	};
}