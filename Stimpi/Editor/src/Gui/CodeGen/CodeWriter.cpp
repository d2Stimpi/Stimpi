#include "stpch.h"
#include "CodeWriter.h"

namespace Stimpi
{

	CodeWriter::CodeWriter()
		: m_FileName(""), m_Data("")
	{
	}

	CodeWriter::~CodeWriter()
	{
	}

	void CodeWriter::Open(const std::string& fileName)
	{
		m_FileName = fileName;
		m_Output.open(m_FileName.c_str());
	}

	void CodeWriter::Close()
	{
		if (m_Output.is_open())
		{
			Flush();
			m_Output.close();
		}
	}

	void CodeWriter::Flush()
	{
		m_Output << m_Data;
		m_Data.clear();
	}

	void CodeWriter::WriteTabs()
	{
		for (int i = 0; i < m_Tabs; i++)
		{
			m_Data.append("\t");
		}
	}

	CodeWriter& CodeWriter::operator<<(std::ostream& (*fn)(std::ostream&))
	{
		m_Data.append("\n");
		WriteTabs();
		return *this;
	}

	template <>
	CodeWriter& operator<<<CodeManip>(CodeWriter& cw, CodeManip manip)
	{
		switch (manip)
		{
		case CodeManip::BlockBegin:
		{
			cw.m_Tabs++;
			cw << "{" << std::endl;
			break;
		}
		case CodeManip::BlockEnd:
		{
			cw.m_Tabs--;
			// Remove last tab that was added after last std::endl
			cw.m_Data.pop_back();
			cw << "}" << std::endl;
			break;
		}
		default: break;
		}

		return cw;
	}

	template <>
	CodeWriter& operator<<<pin_type_variant>(CodeWriter& cw, pin_type_variant val)
	{
		if (std::holds_alternative<bool>(val))
		{
			cw << std::get<bool>(val);
		}

		if (std::holds_alternative<int>(val))
		{
			cw << std::get<int>(val);
		}

		if (std::holds_alternative<float>(val))
		{
			cw << std::get<float>(val);
		}

		if (std::holds_alternative<glm::vec2>(val))
		{
			glm::vec2 vec = std::get<glm::vec2>(val);
			cw << vec.x << ", " << vec.y;
		}

		if (std::holds_alternative<std::string>(val))
		{
			cw << std::get<std::string>(val);
		}

		return cw;
	}

}