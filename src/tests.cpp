#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "bitfield.hpp"
#include "Manager.hpp"

#include <string>

// ******************
//
// TODO: Figure out why Catch2 causes linker errors when having multiple *_tests.cpp files.
// 
// Example: bitfield_tests.cpp & manager_tests.cpp will fail to link due to bitfield redefintion, despite having #pragma once in the header
//
// ******************



// ******************
//
//	BITFIELD TESTS
//
// ******************

TEST_CASE("bitfield SET", "[bitfield]")
{
	bitfield::Bitfield bits = 0b0000;

	// set one bit and verify
	bits = bitfield::Set(bits, 0b0001);
	REQUIRE(bits == 0b0001);

	// set another bit and verify both bits are set
	bits = bitfield::Set(bits, 0b0100);
	REQUIRE(bits == 0b0101);

	// set remaining two bits at once and verify all bits are set
	bits = bitfield::Set(bits, 0b1010);
	REQUIRE(bits == 0b1111);
}


TEST_CASE("bitfield CLEAR", "[bitfield]")
{
	// start with two bits set
	bitfield::Bitfield bits = 0b1100;

	// clear one bit and verify it's gone
	bits = bitfield::Clear(bits, 0b1000);
	REQUIRE(bits == 0b0100);

	// clear a bit that isn't set and verify nothing changed
	bits = bitfield::Clear(bits, 0b0000);
	REQUIRE(bits == 0b0100);
}

TEST_CASE("bitfield HAS", "[bitfield]")
{
	// start with two bits set
	bitfield::Bitfield bits = 0b1100;

	// verify that 0b0100 is set
	REQUIRE(bitfield::Has(bits, 0b0100));

	// verify that 0b1000 is set
	REQUIRE(bitfield::Has(bits, 0b1000));

	// verify that both are set at the same time
	REQUIRE(bitfield::Has(bits, 0b1100));

	// verify we fail if we're looking for a 3rd bit to be set
	REQUIRE(bitfield::Has(bits, 0b1101) == false);
}

// ******************
//
//	MANAGER TESTS
//
// ******************

struct Identity
{
	std::string name;
};

struct Health
{
	int max;
	int current;
};

TEST_CASE("manager REGISTER", "[manager]")
{
	ECS ecs;

	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Health());
}

TEST_CASE("manager CREATE", "[manager]")
{
	ECS ecs;

	Entity e0 = ecs.CreateEntity();
	Entity e1 = ecs.CreateEntity();
	Entity e2 = ecs.CreateEntity();
	Entity e3 = ecs.CreateEntity();
	Entity e4 = ecs.CreateEntity();
	
	// e0 will equal 0 and fail the null check.
	REQUIRE(e1 != NULL);
	REQUIRE(e2 != NULL);
	REQUIRE(e3 != NULL);
	REQUIRE(e4 != NULL);
}

TEST_CASE("Manager Happy Path", "[manager]")
{
	ECS ecs;

	ecs.RegisterComponent(Identity());
	ecs.RegisterComponent(Health());

	Entity entity1 = ecs.CreateEntity();

	Health hp { 100, 44 };
	ecs.AddComponent(entity1, hp);

	// Getting component
	Health* storedHp = ecs.GetComponent(entity1, Health());

	REQUIRE(storedHp != NULL);

	REQUIRE(storedHp->current == hp.current);
	REQUIRE(storedHp->max == hp.max);
}
