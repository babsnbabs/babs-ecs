#include "doctest.h"

#include "babs_ecs.hpp"


TEST_CASE("Confirming symlink babs_ecs -> ECSManager works")
{
    babs_ecs::ECSManager ecs;

    babs_ecs::Entity e = ecs.CreateEntity();

    REQUIRE(e.UUID == 1);
}
