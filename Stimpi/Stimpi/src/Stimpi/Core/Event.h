#pragma once

#include "Stimpi/Core/Core.h"

#include <SDL.h>

namespace Stimpi
{
	class ST_API Event
	{
	public:
		Event() : m_Handled(false), m_Event({}) {};
		Event(SDL_Event e) : m_Handled(false), m_Event(e) {};
		virtual ~Event() {};

		void Handled() { m_Handled = true; }
		bool IsHandled() { return m_Handled; }
		void SetEvent(SDL_Event e) { m_Event = e; }

		inline const SDL_Event& GetRawEvent() { return m_Event; }
	private:
		bool m_Handled;
		// Temp encapsualtion of SDL event
		SDL_Event m_Event;
	};
}