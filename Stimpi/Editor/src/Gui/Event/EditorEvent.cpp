#include "stpch.h"
#include "Gui/Event/EditorEvent.h"

namespace Stimpi
{
	EditorEvent* EditorEvent::Create(EditorEventType type, EditorEventData data)
	{
		return new EditorEvent(type, data);
	}
}