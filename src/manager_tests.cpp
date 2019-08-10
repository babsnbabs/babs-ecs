#include "doctest.h"

#include <string>

#include "Manager.hpp"
#include "exceptions/ComponentNotRegisteredException.hpp"

struct Identity
{
	std::string name;
};

struct AI
{
	std::string difficulty;
};

struct Health
{
	int max;
	int current;
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

TEST_CASE("manager REMOVE COMPONENT")
{
	ECS ecs;

	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Health());

	Identity e0Ident = Identity();
	e0Ident.name = "babs2";

	Identity e1Ident = Identity();
	e1Ident.name = "babs1";

	Health e0Health = Health();
	e0Health.current = 100;
	e0Health.max = 100;

	Health e1Health = Health();
	e1Health.current = 90;
	e1Health.max = 90;

	Entity e0 = ecs.CreateEntity();
	Entity e1 = ecs.CreateEntity();
	ecs.AddComponent(e0, e0Ident);
	ecs.AddComponent(e0, e0Health);

	ecs.AddComponent(e1, e1Ident);
	ecs.AddComponent(e1, e1Health);

	// has Health
	auto health = ecs.GetComponent(e0, Health());
	REQUIRE(e0Health.current == health->current);
	REQUIRE(e0Health.max == health->max);

	ecs.RemoveComponent(e0, Health());

	// does not have health anymore
	auto noHealth = ecs.GetComponent(e0, Health());

	REQUIRE(noHealth == nullptr);

	//still has Identity
	auto identity = ecs.GetComponent(e0, Identity());
	REQUIRE(identity->name == "babs2");

	auto entitiesWithHealth = ecs.EntitiesWith(Health());

	REQUIRE(entitiesWithHealth.size() == 1);
}