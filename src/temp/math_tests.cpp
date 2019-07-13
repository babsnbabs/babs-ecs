#include "catch/catch.hpp"

#include "math.hpp"

TEST_CASE("Add adds things", "[math]") {
    REQUIRE(math::add(1, 2) == 3);
}
