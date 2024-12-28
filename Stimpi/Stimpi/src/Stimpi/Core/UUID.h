#pragma once

#include "Stimpi/Core/Core.h"

#include <xhash>

namespace Stimpi
{
	using UUIDType = uint64_t;

	class ST_API UUID
	{
	public:
		UUID();
		UUID(UUIDType uuid);
		UUID(UUID& uuid) = default;

		operator UUIDType() const { return m_UUID; }

	private:
		UUIDType m_UUID;
	};
}

namespace std
{
	template<>
	struct hash<Stimpi::UUID>
	{
		std::size_t operator()(const Stimpi::UUID& uuid) const
		{
			return hash<Stimpi::UUIDType>()((Stimpi::UUIDType)uuid);
		}
	};
}