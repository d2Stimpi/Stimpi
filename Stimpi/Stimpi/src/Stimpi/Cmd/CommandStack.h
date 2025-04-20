#pragma once

#include "Stimpi/Cmd/Commands.h"

namespace Stimpi
{
	class ST_API CommandStack
	{
	public:
		static void Push(Command* cmd);
		static Command* LastCmd();	// For Undo walking the stack
		static Command* PrevCmd();	// For Redo walking the stack

		static void Invalidate();	// When Scene is played for example

		// For testing
		static uint32_t Count();
		static uint32_t Index();
	};
}