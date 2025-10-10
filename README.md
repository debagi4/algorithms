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



