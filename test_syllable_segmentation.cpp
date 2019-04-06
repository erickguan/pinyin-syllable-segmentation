// I want this lib to be simple (single file)
// so I don't use the package manager.
// But it's adsivable to do so if we have more files.

#define CATCH_CONFIG_MAIN

#include <memory>

#include "catch.hpp"
#include "syllable_segmentation.hpp"

namespace epinyin {

using namespace std;
using namespace Catch;

TEST_CASE("LoadSyllableIndex loads the syllables")
{
  auto s = LoadSyllableIndex();
  REQUIRE(s->left.at("fa") > 0);
}

class SyllableSegmentorFixture
{
 protected:
  shared_ptr<SyllableIndexBiMap> syllable_bimap_;

 public:
  SyllableSegmentorFixture() : syllable_bimap_(LoadSyllableIndex()) {}
};

TEST_CASE_METHOD(SyllableSegmentorFixture,
                 "AppendPhone can be appended with more phones", "[unit]")
{
  SyllableSegmentor s(syllable_bimap_);
  REQUIRE_NOTHROW(s.AppendPhone('c'));
}

TEST_CASE_METHOD(SyllableSegmentorFixture, "PopLastPhone can delete last phone",
                 "[unit]")
{
  SyllableSegmentor s(syllable_bimap_);
  REQUIRE_NOTHROW(s.AppendPhone('c'));
  REQUIRE_NOTHROW(s.PopLastPhone());
}

TEST_CASE_METHOD(SyllableSegmentorFixture,
                 "GetSyllableList return a list of syllable list", "[unit]")
{
  string test1 = "fangan";
  SECTION("input " + test1)
  {
    SyllableSegmentor s(syllable_bimap_);
    for (auto c : test1) {
      s.AppendPhone(c);
    }
    auto l = s.GetSyllableList();
    CHECK_THAT(l, VectorContains(string("fang`an")));
    CHECK_THAT(l, VectorContains(string("fan`gan")));
  }

  string test2 = "xiangang";
  SECTION("input " + test2)
  {
    SyllableSegmentor s(syllable_bimap_);
    for (auto c : test2) {
      s.AppendPhone(c);
    }
    auto l = s.GetSyllableList();
    CHECK_THAT(l, VectorContains(string("xi`an`gang")));
    CHECK_THAT(l, VectorContains(string("xian`gang")));
    CHECK_THAT(l, VectorContains(string("xiang`ang")));
    CHECK_THAT(l, VectorContains(string("xi`ang`ang")));
  }
}

TEST_CASE_METHOD(SyllableSegmentorFixture, "can add, query, and delete phones",
                 "[integration]")
{
  SyllableSegmentor s(syllable_bimap_);
  for (auto c : "fangan") {
    s.AppendPhone(c);
  }
  auto l = s.GetSyllableList();
  CHECK_THAT(l, VectorContains(string("fang`an")));
  CHECK_THAT(l, VectorContains(string("fan`gan")));
  s.PopLastPhone();
  s.PopLastPhone();
  auto n = s.GetSyllableList();
  CHECK_THAT(n, VectorContains(string("fang")));
}

};  // namespace epinyin
