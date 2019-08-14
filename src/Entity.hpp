#pragma once
#include "bitfield/bitfield.hpp"

namespace babs_ecs
{
    struct Entity
    {
        bitfield::Bitfield bitfield;
        uint32_t UUID;

        Entity(uint32_t uuid)
        {
            bitfield = 0;
            UUID = uuid;
        }

        bool operator==(Entity other)
        {
            return this->UUID == other.UUID;
        }
    };
}
