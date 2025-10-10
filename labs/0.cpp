#include <iostream>
#include <string>

namespace {
bool IsSquareString(const std::string& input_string) {
  if (input_string.length() % 2 != 0) {
    return false;
  }

  const std::size_t length = input_string.length();
  for (std::size_t left = 0; left < length / 2; left++) {
    const std::size_t right = ((length + 1) / 2) + left;
    if (input_string[left] != input_string[right]) {
      return false;
    }
  }
  return true;
}
}  // namespace

int main() {
  int count = 0;
  if (std::cin >> count) {
    std::string input;
    for (int i = 0; i < count; i++) {
      if (std::cin >> input) {
        std::cout << (IsSquareString(input) ? "YES" : "NO") << '\n';
      }
    }
  }

  return 0;
}
