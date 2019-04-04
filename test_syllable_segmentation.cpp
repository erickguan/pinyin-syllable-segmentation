// I want this lib to be simple (single file)
// so I don't use the package manager.
// But it's adsivable to do so if we have more files.

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

TEST_CASE( "Factorials are computed", "[factorial]") {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}
