#include "doctest.h"

#include <string>

#include "ECSManager.hpp"
#include "Exceptions.hpp"

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
		babs_ecs::ECSManager ecs;

		babs_ecs::Entity e0 = ecs.CreateEntity();
		babs_ecs::Entity e1 = ecs.CreateEntity();
		ecs.CreateEntity(); // 2
		ecs.CreateEntity(); // 3
		babs_ecs::Entity e4 = ecs.CreateEntity();

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
	babs_ecs::ECSManager ecs;
	babs_ecs::Entity e = ecs.CreateEntity();

	TEST_CASE("AddComponent with unregistered component should throw")
	{
		CHECK_THROWS_AS(ecs.AddComponent(e, Health()), const babs_ecs::ComponentNotRegisteredException);
	}

	TEST_CASE("AddComponent works after component is registered")
	{
		ecs.RegisterComponent<Health>();
		CHECK_NOTHROW(ecs.AddComponent(e, Health{ 10, 5 }));
	}

	TEST_CASE("HasComponent can find added components")
	{
		ecs.RegisterComponent<Identity>();

		REQUIRE(ecs.HasComponent<Health>(e) == true);
		REQUIRE(ecs.HasComponent<Identity>(e) == false);
	}

	TEST_CASE("GetComponent returns expected data")
	{
		Health* h = ecs.GetComponent<Health>(e);
		REQUIRE(h->max == 10);
		REQUIRE(h->current == 5);
	}

	TEST_CASE("Modified component data persists")
	{
		Health* h = ecs.GetComponent<Health>(e);
		h->max = 15;
		h->current = 12;

		h = ecs.GetComponent<Health>(e);
		REQUIRE(h->max == 15);
		REQUIRE(h->current == 12);
	}

	TEST_CASE("GetComponent with unregistered component throws")
	{
		CHECK_THROWS_AS(ecs.GetComponent<AI>(e), const babs_ecs::ComponentNotRegisteredException);
	}

	TEST_CASE("RemoveComponent removes the data")
	{
		// add an identity
		ecs.AddComponent(e, Identity{ "babs2" });

		// remove the existing health
		ecs.RemoveComponent<Health>(e);

		// verify health is gone
		REQUIRE(ecs.HasComponent<Health>(e) == false);
		REQUIRE(ecs.GetComponent<Health>(e) == nullptr);

		// still has Identity
		auto identity = ecs.GetComponent<Identity>(e);
		REQUIRE(identity->name == "babs2");
	}

	TEST_CASE("Delete component data that doesn't exist")
	{
		babs_ecs::ECSManager ecs;

		ecs.RegisterComponent<Identity>();
		ecs.RegisterComponent<Health>();

		babs_ecs::Entity e0 = ecs.CreateEntity();
		Identity e0Ident = Identity();
		e0Ident.name = "babs1";

		// Nothing to really assert, it just shouldn't blow up.
		ecs.RemoveComponent<Health>(e0);
	}
}

TEST_SUITE("Manager Searching")
{
	babs_ecs::ECSManager ecs;

	TEST_CASE("EntitiesWith finds 1 entity with Identity")
	{
		ecs.RegisterComponent<Identity>();
		ecs.RegisterComponent<Health>();

		// set up entity 1
		babs_ecs::Entity entity1 = ecs.CreateEntity();

		Identity ident{ "babs1" };
		ecs.AddComponent(entity1, ident);

		Health hp1{ 100, 44 };
		ecs.AddComponent(entity1, hp1);

		// set up entity 2
		babs_ecs::Entity entity2 = ecs.CreateEntity();
		Health hp2{ 50, 22 };
		ecs.AddComponent(entity2, hp2);

		// set up entity 3
		babs_ecs::Entity entity3 = ecs.CreateEntity();

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
		babs_ecs::Entity entity1 = ecs.CreateEntity();
		Health hp{ 100, 44 };
		ecs.AddComponent<Health>(entity1, hp);

		// Getting component
		Health* storedHp = ecs.GetComponent<Health>(entity1);

		REQUIRE(storedHp != nullptr);
		REQUIRE(storedHp->current == hp.current);
		REQUIRE(storedHp->max == hp.max);

		ecs.RegisterComponent<Health>();
		ecs.RegisterComponent<Identity>();

		babs_ecs::Entity e = ecs.CreateEntity();

		// Should throw because AI is not registered
		CHECK_THROWS_AS(ecs.EntitiesWith(Health{}, Identity{}, AI{}), const babs_ecs::ComponentNotRegisteredException);
	}

	TEST_CASE("Manager EntitiesWith - multiple entities")
	{
		babs_ecs::ECSManager ecs;

		babs_ecs::Entity entity = ecs.CreateEntity();
		Identity ident;
		ident.name = "babs1";
		Health hp;
		hp.current = 100;
		hp.max = 100;

		ecs.RegisterComponent<Identity>();
		ecs.RegisterComponent<Health>();

		ecs.AddComponent(entity, ident);
		ecs.AddComponent(entity, hp);

		auto healthAndIdentity = ecs.EntitiesWith(Identity(), Health());

		REQUIRE(healthAndIdentity.size() == 1);
	}
}

TEST_SUITE("Manager deleting entities")
{
	TEST_CASE("Deleting entities reuse their indexRegular delete")
	{
		babs_ecs::ECSManager ecs;

		babs_ecs::Entity zero = ecs.CreateEntity(); // 0
		babs_ecs::Entity one = ecs.CreateEntity(); // 1
		babs_ecs::Entity two = ecs.CreateEntity(); // 2
		babs_ecs::Entity three = ecs.CreateEntity(); // 3

		ecs.RemoveEntity(two);

		babs_ecs::Entity newTwo = ecs.CreateEntity(); // should be 2 again
		REQUIRE(newTwo.UUID == 2);
		babs_ecs::Entity four = ecs.CreateEntity();
		REQUIRE(four.UUID == 4);
	}

	TEST_CASE("Deleting an entity also deletes component data")
	{
		babs_ecs::ECSManager ecs;

		ecs.RegisterComponent<Identity>();

		babs_ecs::Entity e0 = ecs.CreateEntity();
		Identity e0Ident;
		e0Ident.name = "babs1";

		ecs.AddComponent(e0, e0Ident);

		babs_ecs::Entity e1 = ecs.CreateEntity();
		Identity e1Ident;
		e0Ident.name = "babs2";

		ecs.AddComponent(e1, e1Ident);

		ecs.RemoveEntity(e0);

		REQUIRE(ecs.EntitiesWith(Identity()).size() == 1);
	}

	TEST_CASE("Deleting an entity also deletes component data (bigger example)")
	{
		babs_ecs::ECSManager ecs;

		ecs.RegisterComponent<Identity>();
		ecs.RegisterComponent<Health>();

		babs_ecs::Entity e0 = ecs.CreateEntity();
		Identity e0Ident = Identity();
		e0Ident.name = "babs1";
		Health e0Health = Health();
		e0Health.max = 100;
		e0Health.current = 100;

		ecs.AddComponent(e0, e0Ident);
		ecs.AddComponent(e0, e0Health);

		babs_ecs::Entity e1 = ecs.CreateEntity();
		Identity e1Ident = Identity();
		e1Ident.name = "babs2";
		Health e1Health = Health();
		e1Health.max = 500;
		e1Health.current = 500;

		ecs.AddComponent(e1, e1Ident);
		ecs.AddComponent(e1, e1Health);


		babs_ecs::Entity e2 = ecs.CreateEntity();
		Identity e2Ident = Identity();
		e2Ident.name = "noname";
		Health e2Health = Health();
		e2Health.max = 20;
		e2Health.current = 15;

		ecs.AddComponent(e2, e2Ident);
		ecs.AddComponent(e2, e2Health);

		ecs.RemoveEntity(e1);

		REQUIRE(ecs.EntitiesWith(Identity(), Health()).size() == 2);
	}

	TEST_CASE("Deleting an entity and trying to access its data after")
	{
		babs_ecs::ECSManager ecs;

		ecs.RegisterComponent<Identity>();
		ecs.RegisterComponent<Health>();

		babs_ecs::Entity e0 = ecs.CreateEntity();
		Identity e0Ident = Identity();
		e0Ident.name = "babs1";
		Health e0Health = Health();
		e0Health.max = 100;
		e0Health.current = 100;

		ecs.RemoveEntity(e0);

		Identity* ident = ecs.GetComponent<Identity>(e0);

		REQUIRE(ident == nullptr);
	}
}