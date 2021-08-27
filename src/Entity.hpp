#pragma once
#include "bitfield/bitfield.hpp"

namespace babs_ecs
{
    struct Entity
    {
        bitfield::Bitfield bitfield;
        uint32_t UUID;

        Entity() : Entity(0) {}

        Entity(uint32_t uuid)
        {
            bitfield = 0;
            UUID = uuid;
        }

        bool operator==(Entity other)
        {
            return this->UUID == other.UUID;
        }
        
        bool operator<(const Entity &rhs ) const
        {
            return (this->UUID < rhs.UUID);
        }
    };
}
