#pragma once

#include "Stimpi/Cmd/Command.h"

namespace Stimpi
{
	class ST_API TranslateCommand : public Command
	{
	public:
		TranslateCommand(Entity entity, glm::vec3 value)
			: Command(CommandType::TRANSLATE), m_Entity(entity), m_Value(value)
		{}

		void Execute() override;

		static CommandType GetType() { return CommandType::TRANSLATE; }
		static TranslateCommand* Create(Entity entity, glm::vec3 value);
	private:
		Entity m_Entity;
		glm::vec3 m_Value;
	};
}