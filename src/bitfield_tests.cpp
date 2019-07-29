#include "doctest.h"
#include "bitfield.hpp"

TEST_CASE("bitfield SET")
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


TEST_CASE("bitfield CLEAR")
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

TEST_CASE("bitfield HAS")
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
