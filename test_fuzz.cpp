#include "syllable_segmentation.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
  static auto index = epinyin::SyllableIndex::CreateShared("syllable_list.csv");
  epinyin::SyllableSegmentor s(index);

  for (auto i = 0; i < Size; ++i) {
    s.AppendPhone(Data[i]);
  }
  s.GetSyllableList();

  return 0;  // Non-zero return values are reserved for future use.
}
