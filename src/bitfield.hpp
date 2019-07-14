#pragma once

namespace
{
	typedef uint32_t Bitfield;
	
	Bitfield Set(Bitfield field, Bitfield flag)
	{
		return field | flag;
	}

	Bitfield Clear(Bitfield field, Bitfield flag)
	{
		return field ^ flag;
	}

	bool Has(Bitfield field, Bitfield flag)
	{
		return (field&flag) != 0;
	}

	bool HasAll(Bitfield field, Bitfield flags)
	{
		uint32_t result = field & flags;
		return (field & flags) == flags;
	}
}
