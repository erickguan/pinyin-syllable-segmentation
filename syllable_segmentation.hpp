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

#include <ctype>
#include <forward_list>

const auto EmptyPhone = '\0';

struct Phone
{
  vector<Syllable> syllables_;
  char phone_ = EmptyPhone;
  bool empty() const { return phone_ == EmptyPhone; }
};

struct Syllable
{
  int32_t phone_idx_ = -1;
  int16_t syllable_idx_ = -1;
};

const auto kNumRootPhoneElement = 1;
const auto kMaxPhoneLength = 5;
const auto kPinYinSeparator = '`';

/*
 * Creates a SyllableSegmentor to split syllables.
 */
class SyllableSegmentor
{
 public:
  explicit SyllableSegmentor() = default;
  SyllableSegmentor(const SyllableSegmentor& rhs) = delete;
  void operator=(const SyllableSegmentor& rhs) = delete;

  optional<int16_t> getSyllableIndex(const std::string& syllable) {}

  void AppendPhone(char phone)
  {
    auto idx = phones_.count();
    phones_.push_back(phone);
    int num_phones = 0;
    std::vector<char> stack;
    for (auto iter = phones_.crbegin();
         !iter->empty() && iter != phones_.crend() &&
         num_phones < kMaxPhoneLength;
         iter++, ++num_phones) {
      stack.push_back(*iter);
      std::string possible_syllables(stack.crbegin(), stack.crend());
      if (auto syllable_idx = getSyllableIndex(possible_syllables)) {
        // stored in the phone node before the current phone in the stack
        std::next(iter)->syllables.emplace_back(idx, syllable_idx);
      }
    }
  }

  std::vector<std::string> GetSyllableList()
  {
    std::vector<std::string> result;
    std::vector<Syllable*> stack;

    if (!phones_.front().syllables_.empty()) {
      stack.push_back(phones_.front().syllables_.front());
    }
    while (!stack.empty()) {
      std::string syllable_list = std::transform_reduce(
          stack.crbegin(), stack.crend(), "",
          [](const std::string& a, const std::string& b) {
            return a + kPinYinSeparator + b;
          },
          [](const Syllable* s) {  // return a string of correspoding syllable
          });

      auto t = stack.front();
      if (!phones_[t.phone_idx].syllables_.empty()) {
        s.push(&phones_[t.phone_idx].syllables_.front());
      } else if (t->phone_idx_ == phones_.count() - 1) { // points to the end of phones
        // collects the string and leave
      }
      do {
        if ()
      }
      do {
        if (cur->next_ == nullptr) {
          cur = s.top();
          s.pop();
        }
      } while (!s.empty() && (cur->next_ == nullptr));
      if (cur != nullptr) {
        cur = cur->next_;
      }
    }
    return result;
  }

 private:
  std::vector<Phone> phones_(kNumRootPhoneElement);
};

void DeletePhoneNode(PhoneNode* phone_node)
{
  auto p = phone_node->prev_;
  while (p != nullptr) {
    auto cur = p->syllable_node_;
    while (cur != nullptr) {
      if (cur->phone_node_ == phone_node) {
        DeleteCandidateNode(cur);
      }
      cur = cur->next_;
    }
    p = p->prev_;
  }
  pHeadList = pHeadList->pPrePYNode;
}
