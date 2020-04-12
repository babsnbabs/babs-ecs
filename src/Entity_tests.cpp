#include "doctest.h"

#include "ECSManager.hpp"

#include <map>


TEST_SUITE("Entity class")
{
	TEST_CASE("Entity() returns a dummy/non-tracked entity")
	{
		babs_ecs::Entity e = babs_ecs::Entity();

		REQUIRE(e.bitfield == 0);
		REQUIRE(e.UUID == 0);
	}

    TEST_CASE("Entity can be used in a map container")
    {
        std::unordered_map<char, babs_ecs::Entity> items = {};
        items['a'] = babs_ecs::Entity(5);

        REQUIRE(items['a'].UUID == 5);
    }
}
