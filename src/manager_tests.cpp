#include "catch/catch.hpp"

#include "Manager.hpp"

TEST_CASE("Manager Happy Path", "[manager]")
{
	ECS ecs;

	Entity entity1 = ecs.CreateEntity();

	REQUIRE(entity1 == 0);
}