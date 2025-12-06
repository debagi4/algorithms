# Задача A

[Условие](./resources/a.pdf)

- Q - количество запросов
- N - длина вектора чисел

Рассмотренные варианты решения:

- "В лоб" линейно ищем k-й ноль (O(Q*N));

```c++
void UpdateVector(std::vector<int>* vector, const int index, const int value) {
  (*vector)[index] = value;
}

int SeekZero(const std::vector<int>& vector, const int left, const int right, const int k_number) {
  int counter = 0;
  for (int i = left; i <= right; ++i) {
    if (vector[i] == 0) {
      counter++;
      if (counter == k_number) {
        return i;
      }
    }
  }
  return -1;
}
```

- Дерево отрезков. Храним кол-во нулей на отрезке (O(Q*logN));

```c++
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
        Update(next_vertex, left, middle, position, new_value);
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
```

Идея заключается в разделении задачи от общей к частному.
В каждой ноде дерева нетрудно узнать количество нулей в левой 
и правой части, поэтому мы можем углубляться в одну из веток 
для поиска k-го нуля.

Вычислительная сложность: O(Q*N)
Пространственная сложность: O(4*N)

---

# Задача B

[Условие](./resources/b.pdf)

```c
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
```

Идея решения заключается в хранении количества активных битов
в двоичной записи числа в виде мапы.
На событиях обновления количества нужно пройтись по разрядам
числа, чтобы избавиться от остатков и заемов разрядов.

N - количество запросов на изменение битов
K - максимальный значение ключа в мапе

Вычислительная сложность: O(N*K)
Пространственная сложность: O(K)

---

# Задача C

[Условие](./resources/c.pdf)

```c++
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <unordered_map>
#include <vector>

namespace youndex::express {
using namespace __gnu_pbds;

struct Item {
  std::uint64_t id;
  std::uint16_t score;
};

struct CompareByScoreId {
  bool operator()(const Item& a, const Item& b) const noexcept {
    if (a.score != b.score)
      return a.score < b.score;
    return a.id < b.id;
  }
};

template <typename T, typename Compare>
using ordered_set = tree<T, null_type, Compare, rb_tree_tag, tree_order_statistics_node_update>;

/// NB: Is not thread safe.
class ItemFeed final {
public:
  [[nodiscard]] std::vector<std::uint64_t> GetAtPosition(
      const std::size_t position, const std::size_t limit
  ) const {
    assert(position < Size());
    assert(1 <= limit && limit <= 16);

    std::vector<std::uint64_t> result;
    result.reserve(limit);

    for (std::size_t i = 0; i < limit; ++i) {
      auto iter = items_.find_by_order(position + i);
      if (iter == items_.end()) {
        break;
      }
      result.push_back(iter->id);
    }
    return result;
  }

  [[nodiscard]] std::vector<std::uint64_t> GetAtItemWithId(
      const std::uint64_t uid, const std::size_t limit
  ) const {
    assert(1 <= limit && limit <= 16);
    assert(Contains(uid));

    std::vector<std::uint64_t> result;
    result.reserve(limit);

    const auto score = id_to_score_.at(uid);
    Item const key{.id = uid, .score = score};

    auto iter = items_.lower_bound(key);
    for (std::size_t i = 0; i < limit && iter != items_.end(); ++i, ++iter) {
      result.push_back(iter->id);
    }

    return result;
  }

  void Add(Item item) {
    assert(!Contains(item.id));

    items_.insert(item);
    id_to_score_[item.id] = item.score;
  }

  void Update(Item item) {
    assert(Contains(item.id));

    const auto old_score = id_to_score_[item.id];
    items_.erase(Item{item.id, old_score});
    items_.insert(item);
    id_to_score_[item.id] = item.score;
  }

  void Remove(const std::uint64_t uid) {
    assert(Contains(uid));

    const auto score = id_to_score_[uid];
    items_.erase(Item{uid, score});
    id_to_score_.erase(uid);
  }

private:
  [[nodiscard]] bool Contains(std::uint64_t uid) const {
    assert(1 <= uid);

    return id_to_score_.contains(uid);
  }

  [[nodiscard]] std::size_t Size() const {
    return items_.size();
  }

  ordered_set<Item, CompareByScoreId> items_;
  std::unordered_map<std::uint64_t, std::uint16_t> id_to_score_;
};
}  // namespace youndex::express
```

Идея решения в использовании Order static tree - реализации B-tree,
на основе которого работают индексы в базах данных.

Было решено использовать готовую реализацию __gnu_pbds::tree

Пространственная сложность: O(N)
Вычислительная сложность:

| Операция          | Сложность        |
| ----------------- | ---------------- |
| `Add`             | O(log n)         |
| `Update`          | O(log n)         |
| `Remove`          | O(log n)         |
| `GetAtPosition`   | O(log n + limit) |
| `GetAtItemWithId` | O(log n + limit) |


---

# Задача D

[Условие](./resources/d.pdf)

N - длина последовательности чисел
R - количество запросов

- "В лоб". Сложность (O(N*R))

```c++
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
```

- Дерево отрезков. (O(R * logN))

Пространственная сложность: O(N) — храним элементы в двух структурах (позиции нечётные и чётные).

Вычислительная сложность: O((N + R) * log N): построение: O(N log N), запросы: O(log N)

Идея решения: Заметим, что операция типа 1 x y делает попарные обмены.
Это всегда обмен элемента с нечётной позиции с соседним элементом с чётной позиции,
то есть операция сводится к обмену двух равных по длине блоков между последовательностями нечётных и чётных позиций.

Разделяем массив на две последовательности и храним их в декартовых деревьях (implicit treap) по позициям.

```c++
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
```

---

# Задача F

[Условие](./resources/e.pdf)

N — количество вершин (мест) в дереве
f — вершина с цветочным магазином

Пространственная сложность: O(N) — храним элементы в двух структурах (позиции нечётные и чётные).

Вычислительная сложность: O(N*B), где B = 31

Идея решения: Пусть dist\[v] — XOR-сумма весов на пути из f в v.

В дереве путь между любыми двумя вершинами единственный, поэтому XOR для маршрута "x → f → y" равен:

```
(x -> f) XOR (f -> y) = dist[x] XOR dist[y]
```

Требуется выбрать x != y, x != f, y != f, чтобы максимизировать dist\[x] XOR dist\[y]

```c++

constexpr int KMaxBits = 30;

struct Graph {
private:
  std::vector<std::vector<std::pair<int, std::uint32_t> > > graph_;
  std::vector<std::uint32_t> xor_val_;
  int length_ = 0;

  void ComputeXorDistancesIterative(const int root) {
    xor_val_.assign(length_ + 1, 0);

    std::vector<int> parent(length_ + 1, 0);
    std::vector<int> stack_vertices;
    stack_vertices.reserve(length_);

    parent[root] = -1;
    stack_vertices.push_back(root);

    while (!stack_vertices.empty()) {
      const int vertex = stack_vertices.back();
      stack_vertices.pop_back();

      for (const auto& node : graph_[vertex]) {
        const auto next_vertex = node.first;
        const auto edge_weight = node.second;
        if (next_vertex == parent[vertex]) {
          continue;
        }
        parent[next_vertex] = vertex;
        xor_val_[next_vertex] = xor_val_[vertex] ^ edge_weight;
        stack_vertices.push_back(next_vertex);
      }
    }
  }

  struct Trie {
    friend struct Graph;

    struct Node {
      friend struct Trie;

    private:
      std::array<int, 2> child_;

    public:
      Node()
          : child_{
                {-1, -1}
      } {
      }
    };

  private:
    std::vector<Node> nodes_;

  public:
    Trie() {
      nodes_.emplace_back();
    }

    void Insert(const std::uint32_t value) {
      int node_index = 0;
      for (int i = KMaxBits; i >= 0; --i) {
        const std::uint32_t bit = (value >> static_cast<std::uint32_t>(i)) & 1U;
        int& next = nodes_.at(node_index).child_.at(bit);
        if (next == -1) {
          next = static_cast<int>(nodes_.size());
          nodes_.emplace_back();
        }
        node_index = next;
      }
    }

    std::uint32_t QueryMaxXor(std::uint32_t value) const {
      int node_index = 0;
      std::uint32_t res = 0;
      for (int i = KMaxBits; i >= 0; --i) {
        const std::uint32_t bit = (value >> static_cast<std::uint32_t>(i)) & 1U;
        const std::uint32_t want = bit ^ 1U;
        const int preferred_child = nodes_.at(node_index).child_.at(want);
        const int same_child = nodes_.at(node_index).child_.at(bit);
        if (preferred_child != -1) {
          res |= (1U << static_cast<std::uint32_t>(i));
          node_index = preferred_child;
        } else {
          node_index = same_child;
        }
      }
      return res;
    }
  };

public:
  explicit Graph(const int n) : graph_(n + 1), length_(n) {
  }

  void AddEdge(const int from_index, const int to_index, const int weight) {
    graph_[from_index].emplace_back(to_index, static_cast<std::uint32_t>(weight));
    graph_[to_index].emplace_back(from_index, static_cast<std::uint32_t>(weight));
  }

  std::uint32_t GetMaxXorViaFlower(const int flower_vertex) {
    ComputeXorDistancesIterative(flower_vertex);

    Trie trie;
    trie.nodes_.reserve(static_cast<std::size_t>(length_) * (KMaxBits + 1));

    for (int vertex = 1; vertex <= length_; ++vertex) {
      if (vertex == flower_vertex) {
        continue;
      }
      trie.Insert(xor_val_[vertex]);
    }

    std::uint32_t ans = 0;
    for (int vertex = 1; vertex <= length_; ++vertex) {
      if (vertex == flower_vertex) {
        continue;
      }
      ans = std::max<std::uint32_t>(ans, trie.QueryMaxXor(xor_val_[vertex]));
    }
    return ans;
  }
};
```

---

# Задача F

[Условие](./resources/f.pdf)

N - Длина текста
M - Количество слов

Пространственная сложность: O(1)
Вычислительная сложность: O(N)

Идея решения:
Используем два указателя: первый на позицию в тексте, второй - в текущем слове.

Если слово не закончилось - NO
Если дошли до конца текста - NO

```c++
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
```


