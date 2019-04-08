#include "syllable_segmentation.hpp"

int main()
{
  epinyin::SyllableSegmentor s(epinyin::SyllableIndex::CreateShared("syllable_list.csv"));
  for (auto c : "fangan") {
    s.AppendPhone(c);
  }
  auto l = s.GetSyllableList();
  s.PopLastPhone();
  s.PopLastPhone();
  auto size = s.size();
  auto n = s.GetSyllableList();

  return 0;
}
