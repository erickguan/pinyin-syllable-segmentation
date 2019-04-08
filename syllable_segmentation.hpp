// Copyright 2019 Erick Guan
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions: The above copyright
// notice and this permission notice shall be included in all copies or
// substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS",
// WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <cstdint>
#include <exception>
#include <fstream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <boost/bimap.hpp>

namespace epinyin {

const auto EmptyPhone = '\0';
const size_t kNumRootPhoneElement = 1;
const auto kMaxPhoneLength = 5;
const auto kDefaultPinYinSyllableSeparator = '`';

struct Syllable
{
  int32_t phone_idx_ = -1;
  int16_t syllable_idx_ = -1;

  int16_t stored_in_phone_idx_ = -1;
  int16_t pos_ = -1;
  Syllable(int32_t phone_idx, int16_t syllable_idx, int16_t stored_in_phone_idx,
           int16_t pos)
      : phone_idx_(phone_idx),
        syllable_idx_(syllable_idx),
        stored_in_phone_idx_(stored_in_phone_idx),
        pos_(pos)
  {}
};

struct Phone
{
  std::vector<Syllable> syllables_;
  char phone_;
  bool empty() const { return phone_ == EmptyPhone; }
  explicit Phone(char phone = EmptyPhone) : phone_(phone) {}
};

class SyllableIndex
{
  typedef boost::bimap<std::string, int16_t> SyllableIndexBiMap;
  typedef SyllableIndexBiMap::value_type SyllableIndexBiMapPosition;

 public:
  SyllableIndex(const std::string& path)
  {
    std::fstream fin(path, fin.in);
    if (!fin.is_open()) {
      throw std::invalid_argument("Invalid path to load syllables from " +
                                  path);
    } else {
      std::string line;
      getline(fin, line);  // skip header

      int16_t cur_idx = 0;
      while (getline(fin, line)) {
        std::istringstream line_ss(line);
        std::string syllable_str;
        if (getline(line_ss, syllable_str, ',')) {
          index_.insert(SyllableIndexBiMapPosition(syllable_str, cur_idx++));
        }
      }
    }

    if (index_.size() <= 0) {
      throw std::invalid_argument("Syllable maps are empty.");
    }
  }

  static std::shared_ptr<SyllableIndex> CreateShared(const std::string& path)
  {
    return std::make_shared<SyllableIndex>(path);
  }
  std::optional<int16_t> GetIndex(const std::string& syllable) const
  {
    const auto& m = index_.left;
    if (m.find(syllable) == m.end()) {
      return {};
    } else {
      int16_t idx = m.find(syllable)->second;
      return {idx};
    }
  }

  std::optional<std::string> GetSyllable(int16_t idx) const
  {
    const auto& m = index_.right;
    if (m.find(idx) == m.end()) {
      return {};
    } else {
      return {m.find(idx)->second};
    }
  }

 private:
  SyllableIndexBiMap index_;
};

/*
 * Creates a SyllableSegmentor to split syllables.
 */
class SyllableSegmentor
{
 public:
  SyllableSegmentor(
      const std::shared_ptr<SyllableIndex>& syllable_index,
      const char syllable_separator = kDefaultPinYinSyllableSeparator)
      : phones_(kNumRootPhoneElement),
        syllable_index_(syllable_index),
        syllable_separator_(std::string(1, syllable_separator))
  {}
  SyllableSegmentor(const SyllableSegmentor& rhs) = delete;
  void operator=(const SyllableSegmentor& rhs) = delete;

  void AppendPhone(char phone)
  {
    if (phone <= '\0') return;

    auto phone_idx = phones_.size();
    phones_.push_back(Phone(phone));
    int num_phones = 0;
    std::vector<char> stack;
    for (auto iter = phones_.rbegin();
         !iter->empty() && iter != phones_.rend() &&
         num_phones < kMaxPhoneLength;
         iter++, ++num_phones) {
      stack.push_back(iter->phone_);
      std::string possible_syllables(stack.crbegin(), stack.crend());
      if (auto syllable_idx = syllable_index_->GetIndex(possible_syllables);
          syllable_idx) {
        // stored in the phone node before the current phone in the stack
        auto next_iter = std::next(iter);
        auto cur_phone_idx =
            std::distance(begin(phones_), next_iter.base()) - 1;
        next_iter->syllables_.push_back(Syllable(phone_idx, *syllable_idx,
                                                 cur_phone_idx,
                                                 next_iter->syllables_.size()));
      }
    }
  }

  inline bool isEndOfSyllableStored(const Syllable* s) const
  {
    return s->pos_ >= phones_[s->stored_in_phone_idx_].syllables_.size() - 1;
  }

  inline std::optional<const Syllable*> nextSyllableInChain(
      const Syllable* s) const
  {
    if (!phones_[s->phone_idx_].syllables_.empty()) {
      return &phones_[s->phone_idx_].syllables_.front();
    } else {
      return {};
    }
  }

  inline std::optional<const Syllable*> nextSyllable(const Syllable* s) const
  {
    const auto& syllables = phones_[s->stored_in_phone_idx_].syllables_;
    if (s->pos_ < syllables.size() - 1) {
      return &syllables[s->pos_ + 1];
    } else {
      return {};
    }
  }

  inline bool isLeafSyllable(const Syllable* s) const
  {
    return s->phone_idx_ >= phones_.size() - 1;
  }

  inline std::string translateSyllableIndex(const Syllable* s) const
  {
    if (auto r = syllable_index_->GetSyllable(s->syllable_idx_); r) {
      return *r;
    } else {
      return {};
    }
  }

  std::vector<std::string> GetSyllableList() const
  {
    std::vector<std::string> results;
    std::vector<const Syllable*> stack;

    const auto& root = phones_.front().syllables_;
    if (root.empty()) {
      return results;
    }
    const Syllable* t = &root.front();
    while (t != nullptr) {
      std::string syllable_list =
          absl::StrJoin(stack, syllable_separator_,
                        [this](std::string* out, const Syllable* s) {
                          out->append(translateSyllableIndex(s));
                        });
      if (auto next_syllable = nextSyllableInChain(t); next_syllable) {
        stack.push_back(t);
        t = *next_syllable;
        continue;
      } else if (isLeafSyllable(t)) {
        auto result = syllable_list.empty()
                          ? translateSyllableIndex(t)
                          : absl::StrCat(syllable_list, syllable_separator_,
                                         translateSyllableIndex(t));
        results.push_back(std::move(result));
        if (!stack.empty()) {
          t = stack.back();
          stack.pop_back();
        }
      }
      while (!stack.empty() && !nextSyllable(t).has_value()) {
        t = stack.back();
        stack.pop_back();
      }
      if (const auto next_syllable = nextSyllable(t); next_syllable) {
        t = *next_syllable;
      } else {
        t = nullptr;
      }
    }
    return results;
  }

  int16_t size() const { return phones_.size() - 1; }

  void PopLastPhone()
  {
    if (phones_.size() <= 1) {
      throw std::out_of_range("Trying poping phones when no phone is stored.");
    }
    auto last_phone_idx = phones_.size() - 1;
    phones_.pop_back();
    for (auto& p : phones_) {
      auto& s = p.syllables_;
      while (!s.empty() && s.back().phone_idx_ == last_phone_idx) {
        s.pop_back();
      }
    }
  }

 private:
  std::vector<Phone> phones_;
  std::shared_ptr<SyllableIndex> syllable_index_;
  std::string syllable_separator_;
};

};  // namespace epinyin
