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

TEST_SUITE("Manager Setup")
{
	TEST_CASE("CreateEntity returns componentless, unique entities")
	{
		ECS ecs;

		Entity e0 = ecs.CreateEntity();
		Entity e1 = ecs.CreateEntity();
		ecs.CreateEntity(); // 2
		ecs.CreateEntity(); // 3
		Entity e4 = ecs.CreateEntity();

		REQUIRE(e0.bitfield == 0);
		REQUIRE(e0.UUID == 0);

		REQUIRE(e1.bitfield == 0);
		REQUIRE(e1.UUID == 1);

		REQUIRE(e4.bitfield == 0);
		REQUIRE(e4.UUID == 4);
	}
}

TEST_SUITE("Manager Components")
{
	ECS ecs;
	Entity e = ecs.CreateEntity();

	TEST_CASE("AddComponent with unregistered component should throw")
	{
		CHECK_THROWS_AS(ecs.AddComponent(e, Health()), const ComponentNotRegisteredException);
	}

	TEST_CASE("AddComponent works after component is registered")
	{
		ecs.RegisterComponent(Health());
		CHECK_NOTHROW(ecs.AddComponent(e, Health{ 10, 5 }));
	}

	TEST_CASE("HasComponent can find added components")
	{
		ecs.RegisterComponent(Identity());

		REQUIRE(ecs.HasComponent(e, Health()) == true);
		REQUIRE(ecs.HasComponent(e, Identity()) == false);
	}

	TEST_CASE("GetComponent returns expected data")
	{
		Health* h = ecs.GetComponent(e, Health());
		REQUIRE(h->max == 10);
		REQUIRE(h->current == 5);
	}

	TEST_CASE("Modified component data persists")
	{
		Health* h = ecs.GetComponent(e, Health());
		h->max = 15;
		h->current = 12;

		h = ecs.GetComponent(e, Health());
		REQUIRE(h->max == 15);
		REQUIRE(h->current == 12);
	}

	TEST_CASE("GetComponent with unregistered component throws")
	{
		CHECK_THROWS_AS(ecs.GetComponent(e, AI()), const ComponentNotRegisteredException);
	}

	TEST_CASE("RemoveComponent removes the data")
	{
		// add an identity
		ecs.AddComponent(e, Identity{ "babs2" });

		// remove the existing health
		ecs.RemoveComponent(e, Health());

		// verify health is gone
		REQUIRE(ecs.HasComponent(e, Health()) == false);
		REQUIRE(ecs.GetComponent(e, Health()) == nullptr);

		// still has Identity
		auto identity = ecs.GetComponent(e, Identity());
		REQUIRE(identity->name == "babs2");
	}
}

TEST_SUITE("Manager Searching")
{
	ECS ecs;

	TEST_CASE("EntitiesWith finds 1 entity with Identity")
	{
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

	TEST_CASE("EntitysWith finds 2 entities with Health")
	{
		REQUIRE(ecs.EntitiesWith(Health{}).size() == 2);
	}

	TEST_CASE("EntitiesWith finds all entities when no components are specified")
	{
		REQUIRE(ecs.EntitiesWith().size() == 3);
	}

	TEST_CASE("EntitiesWith with unregistered component should throw")
	{
		// set up entity 1
		Entity entity1 = ecs.CreateEntity();
		Health hp{ 100, 44 };
		ecs.AddComponent<Health>(entity1, hp);

		// Getting component
		Health* storedHp = ecs.GetComponent(entity1, Health());

		REQUIRE(storedHp != nullptr);
		REQUIRE(storedHp->current == hp.current);
		REQUIRE(storedHp->max == hp.max);

		ecs.RegisterComponent(Health());
		ecs.RegisterComponent(Identity());

		Entity e = ecs.CreateEntity();

		// Should throw because AI is not registered
		CHECK_THROWS_AS(ecs.EntitiesWith(Health{}, Identity{}, AI{}), const ComponentNotRegisteredException);
	}

	TEST_CASE("Manager EntitiesWith - multiple entities")
	{
		ECS ecs;

		Entity entity = ecs.CreateEntity();
		Identity ident;
		ident.name = "babs1";
		Health hp;
		hp.current = 100;
		hp.max = 100;

		ecs.RegisterComponent(Identity());
		ecs.RegisterComponent(Health());

		ecs.AddComponent(entity, ident);
		ecs.AddComponent(entity, hp);

		auto healthAndIdentity = ecs.EntitiesWith(Identity(), Health());

		REQUIRE(healthAndIdentity.size() == 1);
	}
}
