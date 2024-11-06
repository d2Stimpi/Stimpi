#include "stpch.h"
#include "Gui/Cmd/CommandStack.h"

#include "Gui/Cmd/Command.h"

namespace Stimpi
{
	static std::vector<std::shared_ptr<Command>> s_CmdStack(50);
}