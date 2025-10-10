#include <iostream>
#include <unordered_map>

struct BitsView {
private:
  std::unordered_map<int, int> bits_;
  int ones_ = 0;

public:
  void Add(const int value) {
    bits_[value]++;
    int position = value;
    while (bits_[position] == 2) {
      bits_[position] = 0;
      ones_--;
      bits_[++position]++;
    }
    if (bits_[position] == 1) {
      ones_++;
    }
  }
  void Sub(const int value) {
    bits_[value]--;
    int position = value;
    while (bits_[position] == -1) {
      bits_[position] = 1;
      ones_++;
      bits_[++position]--;
    }
    if (bits_[position] == 0) {
      ones_--;
    }
  }
  int Ones() const {
    return ones_;
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int requests = 0;
  if (std::cin >> requests) {
    try {
      BitsView bits_view{};
      int input = 0;
      char operation = '+';
      for (int i = 0; i < requests; i++) {
        std::cin >> operation >> input;
        if (operation == '+') {
          bits_view.Add(input);
        } else {
          bits_view.Sub(input);
        }
        std::cout << bits_view.Ones() << '\n';
      }
    } catch (const std::exception& e) {
      std::cerr << "An exception occurred: " << e.what() << '\n';
      return 1;
    }
  }

  return 0;
}
