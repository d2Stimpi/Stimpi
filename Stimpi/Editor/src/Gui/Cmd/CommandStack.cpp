#include "stpch.h"
#include "Gui/Cmd/CommandStack.h"

namespace Stimpi
{
	static std::vector<std::shared_ptr<Command>> s_CmdStack;

	void CommandStack::Push(Command* cmd)
	{
		s_CmdStack.emplace_back(cmd);
	}

}