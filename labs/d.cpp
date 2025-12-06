#include <iostream>
#include <random>
#include <vector>

struct PrefixCounter {
private:
  class Node {
    friend struct PrefixCounter;

  private:
    int64_t value_ = 0;
    int64_t subtree_sum_ = 0;
    std::uint64_t priority_ = 0;
    int subtree_size_ = 1;
    Node* left_ = nullptr;
    Node* right_ = nullptr;

  public:
    explicit Node(const int64_t value, const std::uint64_t priority)
        : value_(value), subtree_sum_(value), priority_(priority) {
    }

    static int Size(const Node* node) {
      return (node != nullptr) ? node->subtree_size_ : 0;
    }

    static int64_t Sum(const Node* node) {
      return (node != nullptr) ? node->subtree_sum_ : 0;
    }
  };

  std::vector<Node> storage_;
  Node* oddTreap_ = nullptr;
  Node* evenTreap_ = nullptr;

  std::uint64_t pr_counter_ = 0;

  static constexpr std::uint64_t KSplitMixGamma = 0x9e3779b97f4a7c15ULL;
  static constexpr std::uint64_t KMul1 = 0xbf58476d1ce4e5b9ULL;
  static constexpr std::uint64_t KMul2 = 0x94d049bb133111ebULL;
  static constexpr unsigned KShift1 = 30U;
  static constexpr unsigned KShift2 = 27U;
  static constexpr unsigned KShift3 = 31U;

  static std::uint64_t SplitMix64(std::uint64_t value) {
    value += KSplitMixGamma;
    value = (value ^ (value >> KShift1)) * KMul1;
    value = (value ^ (value >> KShift2)) * KMul2;
    return value ^ (value >> KShift3);
  }

  std::uint32_t NextPriority() {
    return static_cast<std::uint32_t>(SplitMix64(pr_counter_++));
  }

  Node* CreateNode(int64_t value) {
    storage_.emplace_back(value, NextPriority());
    return &storage_.back();
  }

  static void Pull(Node* node) {
    node->subtree_size_ = 1 + Node::Size(node->left_) + Node::Size(node->right_);
    node->subtree_sum_ = node->value_ + Node::Sum(node->left_) + Node::Sum(node->right_);
  }

  static Node* Merge(Node* node_a, Node* node_b) {
    if ((node_a == nullptr) || (node_b == nullptr)) {
      return (node_a != nullptr) ? node_a : node_b;
    }
    if (node_a->priority_ > node_b->priority_) {
      node_a->right_ = Merge(node_a->right_, node_b);
      Pull(node_a);
      return node_a;
    }
    node_b->left_ = Merge(node_a, node_b->left_);
    Pull(node_b);
    return node_b;
  }

  static void SplitByCount(
      Node* node, const int elements_count, Node*& left_part, Node*& right_part
  ) {
    if (node == nullptr) {
      left_part = right_part = nullptr;
      return;
    }
    if (Node::Size(node->left_) >= elements_count) {
      SplitByCount(node->left_, elements_count, left_part, node->left_);
      right_part = node;
      Pull(right_part);
    } else {
      SplitByCount(
          node->right_, elements_count - Node::Size(node->left_) - 1, node->right_, right_part
      );
      left_part = node;
      Pull(left_part);
    }
  }

  static int64_t RangeSumInPlace(Node*& treap, const int left, const int right) {
    if (left > right) {
      return 0;
    }
    Node* left_part = nullptr;
    Node* middle_part = nullptr;
    Node* right_part = nullptr;
    SplitByCount(treap, right, middle_part, right_part);
    SplitByCount(middle_part, left - 1, left_part, middle_part);

    const int64_t ans = Node::Sum(middle_part);

    treap = Merge(Merge(left_part, middle_part), right_part);
    return ans;
  }

  static void SwapBlocks(
      Node*& odd_treap, Node*& even_treap, const int odd_pos, const int even_pos, const int len
  ) {
    Node* odd_a = nullptr;
    Node* odd_b = nullptr;
    Node* odd_c = nullptr;
    Node* even_a = nullptr;
    Node* even_b = nullptr;
    Node* even_c = nullptr;

    SplitByCount(odd_treap, odd_pos - 1, odd_a, odd_b);
    SplitByCount(odd_b, len, odd_b, odd_c);

    SplitByCount(even_treap, even_pos - 1, even_a, even_b);
    SplitByCount(even_b, len, even_b, even_c);

    odd_treap = Merge(Merge(odd_a, even_b), odd_c);
    even_treap = Merge(Merge(even_a, odd_b), even_c);
  }

public:
  explicit PrefixCounter(const std::vector<int>& arr) {
    storage_.reserve(arr.size());

    for (int idx = 1; idx <= static_cast<int>(arr.size()); ++idx) {
      Node* node = CreateNode(arr[idx - 1]);
      if ((idx % 2) != 0) {
        oddTreap_ = Merge(oddTreap_, node);
      } else {
        evenTreap_ = Merge(evenTreap_, node);
      }
    }
  }

  void Swap(const int left, const int right) {
    const int len = (right - left + 1) / 2;
    if (len <= 0) {
      return;
    }

    if ((left % 2) != 0) {
      const int pos = (left + 1) / 2;
      SwapBlocks(oddTreap_, evenTreap_, pos, pos, len);
    } else {
      const int even_pos = left / 2;
      const int odd_pos = even_pos + 1;
      SwapBlocks(oddTreap_, evenTreap_, odd_pos, even_pos, len);
    }
  }

  int64_t GetSum(const int left, const int right) {
    int64_t ans = 0;

    const int first_odd = (left % 2 == 1) ? left : left + 1;
    const int last_odd = (right % 2 == 1) ? right : right - 1;
    if (first_odd <= last_odd) {
      const int new_left = (first_odd + 1) / 2;
      const int new_right = (last_odd + 1) / 2;
      ans += RangeSumInPlace(oddTreap_, new_left, new_right);
    }

    const int first_even = (left % 2 == 1) ? left + 1 : left;
    const int last_even = (right % 2 == 1) ? right - 1 : right;
    if (first_even <= last_even) {
      const int new_left = first_even / 2;
      const int new_right = last_even / 2;
      ans += RangeSumInPlace(evenTreap_, new_left, new_right);
    }

    return ans;
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
