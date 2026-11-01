
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "Services.h"

TEST_CASE("Late penalty rules", "[penalty]") {

    LatePenaltyStrategy penalty(0.10);

    SECTION("No penalty when submitted on time") {
        REQUIRE(penalty.apply(100.0, 0) == Catch::Approx(100.0));
    }

    SECTION("No penalty for early submission") {
        REQUIRE(penalty.apply(100.0, -1) == Catch::Approx(100.0));
    }

    SECTION("10 percent penalty for one day late") {
        REQUIRE(penalty.apply(100.0, 1) == Catch::Approx(90.0));
    }

    SECTION("20 percent penalty for two days late") {
        REQUIRE(penalty.apply(100.0, 2) == Catch::Approx(80.0));
    }

    SECTION("30 percent penalty for three days late") {
        REQUIRE(penalty.apply(100.0, 3) == Catch::Approx(70.0));
    }

    SECTION("No mark after more than three days late") {
        REQUIRE(penalty.apply(100.0, 4) == Catch::Approx(0.0));
    }

    SECTION("Penalty does not produce a negative mark") {
        REQUIRE(penalty.apply(5.0, 3) >= 0.0);
    }
}
