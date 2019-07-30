#include "doctest.h"

#include "Manager.hpp"
#include <string>

struct Identity
{
	std::string name;
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

	// e0 will equal 0 and fail the null check.
	//REQUIRE(e1 != NULL);
	//REQUIRE(e2 != NULL);
	//REQUIRE(e3 != NULL);
	//REQUIRE(e4 != NULL);
}

TEST_CASE("Manager ENTITIES WITH")
{
	//ECS ecs;

	//ecs.RegisterComponent(Identity());
	//ecs.RegisterComponent(Health());

 //   // set up entity 1
	//Entity entity1 = ecs.CreateEntity();

 //   Identity ident{ "babs1" };
 //   ecs.AddComponent(entity1, ident);

	//Health hp1{ 100, 44 };
	//ecs.AddComponent(entity1, hp1);

 //   // set up entity 2
	//Entity entity2 = ecs.CreateEntity();
 //   Health hp2{ 50, 22 };
	//ecs.AddComponent(entity2, hp2);

 //   // set up entity 3
 //   Entity entity3 = ecs.CreateEntity();

 //   // run a couple searches based on the above entities
 //   REQUIRE(ecs.EntitiesWith(Identity{}).size() == 1);
 //   REQUIRE(ecs.EntitiesWith(Health{}).size() == 2);
 //   REQUIRE(ecs.EntitiesWith().size() == 3);
}

TEST_CASE("Manager Happy Path")
{
	//ECS ecs;

	//ecs.RegisterComponent(Identity());
	//ecs.RegisterComponent(Health());

	//Entity entity1 = ecs.CreateEntity();

	//Health hp{ 100, 44 };
	//ecs.AddComponent(entity1, hp);

	//// Getting component
	//Health* storedHp = ecs.GetComponent(entity1, Health());
	//
	//REQUIRE(storedHp != nullptr);
	//REQUIRE(storedHp->current == hp.current);
	//REQUIRE(storedHp->max == hp.max);
}
