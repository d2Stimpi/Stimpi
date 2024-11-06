#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	class ST_API AssetHandle
	{
	public:
		AssetHandle() : m_Handle(0) {}

		void Init(uint32_t index)
		{
			assert(m_Handle == 0);
			assert(index <= MAX_INDEX);

			static uint32_t s_MagiCouter = 0;
			if (++s_MagiCouter > MAX_MAGIC)
			{
				s_MagiCouter = 1;
			}

			m_Index = index;
			m_Magic = s_MagiCouter;

		}

		uint32_t GetIndex() { return m_Index; }
		uint32_t GetMagic() { return m_Magic; }
		uint32_t GetHandle() { return m_Handle; }
		bool IsValid() { return m_Handle != 0; }

		operator uint32_t() { return m_Handle; }

	private:
		union
		{
			enum
			{
				INDEX_BITS = 16,
				MAGIC_BITS = 16,

				MAX_INDEX = (1 << INDEX_BITS) - 1,
				MAX_MAGIC = (1 << MAGIC_BITS) - 1
			};

			struct
			{
				unsigned m_Index : INDEX_BITS;
				unsigned m_Magic : MAGIC_BITS;
			};
			uint32_t m_Handle;
		};
	};
}