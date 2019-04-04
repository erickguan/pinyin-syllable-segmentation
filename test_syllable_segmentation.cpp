// I want this lib to be simple (single file)
// so I don't use the package manager.
// But it's adsivable to do so if we have more files.

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "syllable_segmentation.hpp"

TEST_CASE("AppendPhone can be appended with more phones", "[unit]") {
  SyllableSegmentor s;
  REQUIRE_NOTHROW(s.AppendPhone('c'));
}

TEST_CASE("GetSyllableList return a list of syllable list", "[unit]") {
  SyllableSegmentor s1;
  for (auto c : "fangan") {
    s1.AppendPhone(c);
  }
  SECTION("Contains solutions") {
    CHECK_THAT(s1, VectorContains("fang`an"));
    CHECK_THAT(s1, VectorContains("fan`gan"));
  }

  SyllableSegmentor s2;
  for (auto c : "xiangang") {
    s2.AppendPhone(c);
  }
  SECTION("Contains solutions") {
    CHECK_THAT(s2, VectorContains("xi`an`gang"));
    CHECK_THAT(s2, VectorContains("xian`gang"));
    CHECK_THAT(s2, VectorContains("xiang`ang"));
    CHECK_THAT(s2, VectorContains("xi`ang`ang"));
  }

}
