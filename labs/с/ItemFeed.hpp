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
