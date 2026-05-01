#pragma once

#include "Gui/Event/EditorEvent.h"

namespace Stimpi
{
	class EditorEventQueue
	{
	public:
		static void PushEvent(Event* event);
		static bool PollEvent(Event** event);
	};
}