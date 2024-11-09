#include "stpch.h"
#include "Stimpi/Cmd/CommandStack.h"

namespace Stimpi
{
	static constexpr const uint32_t s_StackSize = 50;

	struct CommandStackContext
	{
		std::vector<std::shared_ptr<Command>> m_Commands;
		uint32_t m_CmdIndex = 0;
		uint32_t m_CmdCount = 0;

		CommandStackContext()
			: m_CmdIndex(0), m_CmdCount(0)
		{
			m_Commands.reserve(s_StackSize);
			for (int i = 0; i < s_StackSize; i++)
				m_Commands.emplace_back(nullptr);
		}
	};

	static CommandStackContext s_Context;

	void CommandStack::Push(Command* cmd)
	{
		// First inserted element will be at index 1
		s_Context.m_CmdIndex = ++s_Context.m_CmdIndex % s_StackSize;

		if (s_Context.m_CmdCount < s_StackSize)
			s_Context.m_CmdCount++;

		std::shared_ptr<Command>& elem = s_Context.m_Commands[s_Context.m_CmdIndex];
		elem.reset(cmd);
	}

	Stimpi::Command* CommandStack::Pop()
	{
		if (s_Context.m_CmdIndex == 0)
			s_Context.m_CmdIndex = s_StackSize - 1;
		else
			s_Context.m_CmdIndex--;
		
		if (s_Context.m_CmdCount > 0)
		{
			s_Context.m_CmdCount--;
			return s_Context.m_Commands[s_Context.m_CmdIndex].get();
		}
		else
			return nullptr;
	}

	uint32_t CommandStack::Count()
	{
		return s_Context.m_CmdCount;
	}

	uint32_t CommandStack::Index()
	{
		return s_Context.m_CmdIndex;
	}

}