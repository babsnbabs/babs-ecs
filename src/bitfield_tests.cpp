#include "catch/catch.hpp"

#include "bitfield.hpp"

TEST_CASE("bitfield SET", "[bitfield]") 
{
	Bitfield bits = 0;

	// 00000001
	bits = Set(bits, 1);
	REQUIRE(bits == 1);

	// 00000101
	bits = Set(bits, 4);
	REQUIRE(bits == 5);
}


TEST_CASE("bitfield CLEAR", "[bitfield]")
{
	// 00001100
	Bitfield bits = 12;

	// 00000100
	bits = Clear(bits, 8);
	REQUIRE(bits == 4);

	// 00000000
	bits = Clear(bits, 4);
	REQUIRE(bits == 0);
}

TEST_CASE("bitfield HAS", "[bitfield]")
{
	// 00001100
	Bitfield bits = 12;

	REQUIRE(Has(bits, 8));
	REQUIRE(Has(bits, 4));
	REQUIRE(Has(bits, 1341)); // why does this pass?
}

TEST_CASE("bitfield HASALL", "[bitfield]")
{
	// 00001100
	Bitfield bits = 12;

	REQUIRE(HasAll(bits, 12));
}