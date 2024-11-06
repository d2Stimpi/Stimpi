#pragma once

#include "Stimpi/Scene/Entity.h"

#include <glm/glm.hpp>

#define STATIC_GET_TYPE(Type)	static CommandType GetType() { return Type; }

namespace Stimpi
{
	enum class CommandType { TRANSLATE, SCALE, ROTATE };

	class Command
	{
	public:
		Command(CommandType type) : m_Type(type) {}

		virtual bool IsValid() = 0;

		CommandType GetType() { return m_Type; }

	private:
		CommandType m_Type;
	};



	class TranslateCommand : public Command
	{
	public:
		TranslateCommand(Entity entity, glm::vec3 value)
			: Command(CommandType::TRANSLATE), m_Entity(entity), m_Value(value)
		{}

		STATIC_GET_TYPE(CommandType::TRANSLATE)

	private:
		Entity m_Entity;
		glm::vec3 m_Value;
	};
}