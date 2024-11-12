#include "stpch.h"
#include "Stimpi/Cmd/CommandStack.h"

namespace Stimpi
{
	static constexpr const uint32_t s_StackSize = 50;

	struct CommandStackContext
	{
		std::vector<Command*> m_Commands;
		uint32_t m_CmdIndex;
		uint32_t m_UndoCount;
		uint32_t m_CmdCount;

		CommandStackContext()
			: m_CmdIndex(0), m_UndoCount(0), m_CmdCount(0)
		{
			m_Commands.reserve(s_StackSize);
			for (int i = 0; i < s_StackSize; i++)
				m_Commands.emplace_back(nullptr);
		}

		~CommandStackContext()
		{
			for (auto cmd : m_Commands)
				delete cmd;
		}
	};

	static CommandStackContext s_Context;

	void CommandStack::Push(Command* cmd)
	{
		// When pushing new cmd on the stack we invalidate / reset the undo counter
		s_Context.m_UndoCount = 0;

		// First inserted element will be at index 1
		s_Context.m_CmdIndex = ++s_Context.m_CmdIndex % s_StackSize;

		if (s_Context.m_CmdCount < s_StackSize)
			s_Context.m_CmdCount++;

		// Check for overwrite and perform cleanup
		if (s_Context.m_Commands[s_Context.m_CmdIndex] != nullptr)
		{
			delete s_Context.m_Commands[s_Context.m_CmdIndex];
			s_Context.m_Commands[s_Context.m_CmdIndex] = nullptr;
		}
		s_Context.m_Commands[s_Context.m_CmdIndex] = cmd;
	}

	Stimpi::Command* CommandStack::LastCmd()
	{
		Command* retCmd = nullptr;

		if (s_Context.m_CmdCount > 0)
		{
			s_Context.m_UndoCount++;
			s_Context.m_CmdCount--;
			retCmd = s_Context.m_Commands[s_Context.m_CmdIndex];

			if (s_Context.m_CmdIndex == 0)
				s_Context.m_CmdIndex = s_StackSize - 1;
			else
				s_Context.m_CmdIndex--;
		}
		
		return retCmd;
	}

	Stimpi::Command* CommandStack::PrevCmd()
	{
		Command* retCmd = nullptr;

		if (s_Context.m_UndoCount > 0)
		{
			s_Context.m_UndoCount--;
			s_Context.m_CmdCount++;

			s_Context.m_CmdIndex = ++s_Context.m_CmdIndex % s_StackSize;

			retCmd = s_Context.m_Commands[s_Context.m_CmdIndex];
		}

		return retCmd;
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