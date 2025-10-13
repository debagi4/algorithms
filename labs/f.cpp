#include <iostream>
#include <string>
#include <vector>

namespace {
bool IsAvailable(const std::string& text, const std::vector<std::string>& words) {
  int const text_len = static_cast<int>(text.size());
  int const words_len = static_cast<int>(words.size());
  int current = 0;
  for (int i = 0; i < words_len; i++) {
    const std::string& word = words[i];
    size_t word_index = 0;

    while (current < text_len && word_index < word.size()) {
      if (text[current] == word[word_index]) {
        word_index++;
      }
      current++;
    }

    if (word_index < word.size()) {
      return false;
    }

    if (i != words_len - 1) {
      if (current >= text_len) {
        return false;
      }
      current++;
    }
  }
  return true;
}
}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int symbol_count = 0;
  int word_count = 0;
  if (std::cin >> symbol_count >> word_count) {
    try {
      std::string text;
      std::cin >> text;

      std::vector<std::string> words(word_count);
      for (int i = 0; i < word_count; ++i) {
        std::cin >> words[i];
      }

      if (IsAvailable(text, words)) {
        std::cout << "YES";
      } else {
        std::cout << "NO";
      }
    } catch (const std::exception& e) {
      std::cerr << "An exception occurred: " << e.what() << '\n';
      return 1;
    }
  }

  return 0;
}
