#pragma once

#include "Stimpi/Core/Core.h"

#include <xhash>

namespace Stimpi
{
	class ST_API UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(UUID& uuid) = default;

		operator uint64_t() const { return m_UUID; }

	private:
		uint64_t m_UUID;
	};
}

namespace std
{
	template<>
	struct hash<Stimpi::UUID>
	{
		std::size_t operator()(const Stimpi::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}