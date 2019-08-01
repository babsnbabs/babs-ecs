#include <string>

#include "doctest.h"

#include "Manager.hpp"
#include "exceptions/ComponentNotRegisteredException.hpp"

struct Identity
{
	std::string name;
};

struct Health
{
	int max;
	int current;
};

struct AI
{
	std::string difficulty;
};

TEST_CASE("manager REGISTER")
{
	ECS ecs;

	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Health());
}

TEST_CASE("manager CREATE")
{
	ECS ecs;

	Entity e0 = ecs.CreateEntity();
	Entity e1 = ecs.CreateEntity();
	Entity e2 = ecs.CreateEntity();
	Entity e3 = ecs.CreateEntity();
	Entity e4 = ecs.CreateEntity();

	REQUIRE(e0.bitfield == 0);
	REQUIRE(e0.UUID == 0);

	REQUIRE(e1.bitfield == 0);
	REQUIRE(e1.UUID == 1);

	REQUIRE(e2.bitfield == 0);
	REQUIRE(e2.UUID == 2);

	REQUIRE(e3.bitfield == 0);
	REQUIRE(e3.UUID == 3);

	REQUIRE(e4.bitfield == 0);
	REQUIRE(e4.UUID == 4);
}

TEST_CASE("Manager ENTITIES WITH")
{
	ECS ecs;

	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Health());

    // set up entity 1
	Entity entity1 = ecs.CreateEntity();

    Identity ident{ "babs1" };
    ecs.AddComponent(entity1, ident);

	Health hp1{ 100, 44 };
	ecs.AddComponent(entity1, hp1);

    // set up entity 2
	Entity entity2 = ecs.CreateEntity();
    Health hp2{ 50, 22 };
	ecs.AddComponent(entity2, hp2);

    // set up entity 3
    Entity entity3 = ecs.CreateEntity();

    // run a couple searches based on the above entities
    REQUIRE(ecs.EntitiesWith(Identity{}).size() == 1);
    REQUIRE(ecs.EntitiesWith(Health{}).size() == 2);
    REQUIRE(ecs.EntitiesWith().size() == 3);
}

TEST_CASE("Manager Happy Path")
{
	ECS ecs;

	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Health());

	Entity entity1 = ecs.CreateEntity();

	Health hp{ 100, 44 };
	ecs.AddComponent(entity1, hp);

	// Getting component
	Health* storedHp = ecs.GetComponent(entity1, Health());
	
	REQUIRE(storedHp != nullptr);
	REQUIRE(storedHp->current == hp.current);
	REQUIRE(storedHp->max == hp.max);
}

TEST_CASE("Manager AddComponent with unregistered component throws")
{
	ECS ecs;
	Entity e = ecs.CreateEntity();

	CHECK_THROWS_AS(ecs.AddComponent(e, Health()), const ComponentNotRegisteredException);
}

TEST_CASE("Manager GetComponent with unregistered component throws")
{
	ECS ecs;
	Entity e = ecs.CreateEntity();

	CHECK_THROWS_AS(ecs.GetComponent(e, Health()), const ComponentNotRegisteredException);
}

TEST_CASE("Manager Entities_With with unregistered component throws")
{
	ECS ecs;

	ecs.RegisterComponent(Health());
	ecs.RegisterComponent(Identity());

	Entity e = ecs.CreateEntity();

	// Should throw because AI is not registered
	// However, it doesn't throw at all because it only "sees" health in EntitiesWith - after all the recursion.
	CHECK_THROWS_AS(ecs.EntitiesWith(Health{}, Identity{}, AI{}), const ComponentNotRegisteredException);
}