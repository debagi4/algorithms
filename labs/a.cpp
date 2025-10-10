#include <cstddef>
#include <iostream>
#include <vector>

struct SegmentTree {
private:
  int count_;
  std::vector<int> tree_;

  void Build(const std::vector<int>& arr, const int vertex, const int left, const int right) {
    if (left == right) {
      tree_[vertex] = (arr[left] == 0 ? 1 : 0);
    } else {
      int const middle = (left + right) / 2;
      int const next_vertex = vertex * 2;
      Build(arr, next_vertex, left, middle);
      Build(arr, next_vertex + 1, middle + 1, right);
      tree_[vertex] = tree_[next_vertex] + tree_[next_vertex + 1];
    }
  }

  void Update(
      const int vertex, const int left, const int right, const int position, const int new_value
  ) {
    if (left == right) {
      tree_[vertex] = (new_value == 0 ? 1 : 0);
    } else {
      int const middle = (left + right) / 2;
      int const next_vertex = vertex * 2;
      if (position <= middle) {
        Update(vertex * 2, left, middle, position, new_value);
      } else {
        Update(next_vertex + 1, middle + 1, right, position, new_value);
      }
      tree_[vertex] = tree_[next_vertex] + tree_[next_vertex + 1];
    }
  }

  int ZeroCount(const int vertex, const int left, const int right, const int begin, const int end) {
    if (begin > end) {
      return 0;
    }
    if (begin == left && end == right) {
      return tree_[vertex];
    }
    int const middle = (left + right) / 2;
    int const next_vertex = vertex * 2;
    return ZeroCount(next_vertex, left, middle, begin, std::min(end, middle)) +
           ZeroCount(next_vertex + 1, middle + 1, right, std::max(begin, middle + 1), end);
  }

  int ZeroCount(const int begin, const int end) {
    return ZeroCount(1, 0, count_ - 1, begin, end);
  }

  int FindKthZero(
      const int vertex,
      const int left,
      const int right,
      const int begin,
      const int end,
      const int k_number
  ) {
    if (begin > end) {
      return -1;
    }
    if (left == right) {
      return left;
    }
    int const middle = (left + right) / 2;
    int const next_vertex = vertex * 2;

    int left_count = 0;
    if (begin <= left && right <= end) {
      left_count = tree_[next_vertex];
    } else {
      int const begin_index = std::max(begin, left);
      int const end_index = std::min(end, middle);
      left_count = (begin_index <= end_index)
                       ? ZeroCount(next_vertex, left, middle, begin_index, end_index)
                       : 0;
    }

    if (k_number <= left_count) {
      return FindKthZero(next_vertex, left, middle, begin, std::min(end, middle), k_number);
    }
    return FindKthZero(
        next_vertex + 1, middle + 1, right, std::max(begin, middle + 1), end, k_number - left_count
    );
  }

public:
  void Build(const std::vector<int>& arr) {
    Build(arr, 1, 0, count_ - 1);
  }

  int FindKthZero(const int begin, const int end, const int k_nuber) {
    if (ZeroCount(begin, end) < k_nuber) {
      return -1;
    }
    return FindKthZero(1, 0, count_ - 1, begin, end, k_nuber);
  }

  void Update(const int position, const int new_value) {
    Update(1, 0, count_ - 1, position, new_value);
  }

  explicit SegmentTree(const int n) : count_(n) {
    tree_.assign(static_cast<size_t>(4) * n, 0);
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  if (int count = 0; std::cin >> count) {
    try {
      std::vector<int> arr(count);
      SegmentTree segment_tree(count);

      for (int i = 0; i < count; i++) {
        std::cin >> arr[i];
      }
      segment_tree.Build(arr);

      int queries = 0;
      if (std::cin >> queries) {
        for (int i = 0; i < queries; i++) {
          char command = 'u';
          std::cin >> command;
          switch (command) {
            case 'u': {
              int index = 0;
              int new_value = 0;
              std::cin >> index >> new_value;
              segment_tree.Update(index - 1, new_value);
              break;
            }
            case 's': {
              int k_number = 0;
              int left = 0;
              int right = 0;
              std::cin >> left >> right >> k_number;
              int result = segment_tree.FindKthZero(left - 1, right - 1, k_number);
              if (result != -1) {
                result++;
              }
              std::cout << result << '\n';
              break;
            }
            default:
              break;
          }
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "An exception occurred: " << e.what() << '\n';
      return 1;
    }
  }

  return 0;
}
