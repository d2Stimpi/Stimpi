#pragma once

#include "Gui/Nodes/GraphComponents.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	enum class CodeManip
	{
		// Code block manipulators
		BlockBegin,
		BlockEnd
	};

	class CodeWriter
	{
	public:
		CodeWriter();
		~CodeWriter();

		void Open(const std::string& fileName);
		void Close();

		void Flush();
		CodeWriter& operator<<(std::ostream& (*fn)(std::ostream&));

	private:
		void WriteTabs();

	private:
		std::string m_FileName;
		uint32_t m_Tabs = 0;
		std::string m_Data;

		std::ofstream m_Output;

		template <typename T> friend CodeWriter& operator<<(CodeWriter&, T);
	};

	template <typename T>
	CodeWriter& operator<<(CodeWriter& cw, T data)
	{
		std::stringstream ss("");
		ss << data;

		cw.m_Data.append(ss.str());
		return cw;
	}

	template <>
	CodeWriter& operator<<<CodeManip>(CodeWriter& cw, CodeManip manip);

	template <>
	CodeWriter& operator<<<pin_type_variant>(CodeWriter& cw, pin_type_variant val);
}