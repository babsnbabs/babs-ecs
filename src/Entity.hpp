#pragma once
#include "bitfield.hpp"

struct Entity
{
	bitfield::Bitfield bitfield;
	int32_t UUID;

	Entity(uint32_t uuid)
	{
		bitfield = 0;
		UUID = uuid;
	}
};