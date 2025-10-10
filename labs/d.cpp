#include <iostream>
#include <vector>

struct PrefixCounter {
private:
  std::vector<int> arr_;
  std::vector<int> prefix_;

public:
  explicit PrefixCounter(const std::vector<int>& arr) : arr_(arr) {
    prefix_.assign(arr.size() + 1, 0);
    prefix_[0] = 0;
    prefix_[1] = arr[0];
    for (size_t i = 1; i < arr.size(); i++) {
      prefix_[i + 1] = prefix_[i] + arr[i];
    }
  }

  void Swap(const int left, const int right) {
    for (int j = left; j <= right; j += 2) {
      prefix_[j] += arr_[j] - arr_[j - 1];
      std::swap(arr_[j], arr_[j - 1]);
    }
  }
  int GetSum(const int left, const int right) const {
    return prefix_[right] - prefix_[left - 1];
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int length = 0;
  int requests = 0;
  int suite = 1;
  while (std::cin >> length && std::cin >> requests) {
    if (length == 0 && requests == 0) {
      break;
    }
    if (suite > 1) {
      std::cout << '\n';
    }
    std::cout << "Suite " << suite++ << ":" << '\n';
    try {
      std::vector<int> arr(length);
      for (int i = 0; i < length; i++) {
        std::cin >> arr[i];
      }

      auto prefix_counter = PrefixCounter(arr);

      int type = 1;
      int arg0 = 0;
      int arg1 = 0;
      for (int i = 0; i < requests; i++) {
        std::cin >> type >> arg0 >> arg1;
        if (type == 1) {
          prefix_counter.Swap(arg0, arg1);
        } else {
          std::cout << prefix_counter.GetSum(arg0, arg1) << '\n';
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "An exception occurred: " << e.what() << '\n';
      return 1;
    }
  }

  return 0;
}
