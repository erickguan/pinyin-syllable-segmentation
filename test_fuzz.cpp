#include "syllable_segmentation.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
  epinyin::SyllableSegmentor s(epinyin::SyllableIndex::CreateShared("syllable_list.csv"));

  for (auto i = 0; i < Size; ++i) {
    s.AppendPhone(Data[i]);
  }
  s.GetSyllableList();
  if (s.size() > 1) {
    s.PopLastPhone();
    s.GetSyllableList();
  }

  return 0;  // Non-zero return values are reserved for future use.
}
