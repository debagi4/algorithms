#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

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

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int length = 0;
  if (std::cin >> length) {
    try {
      Graph graph(length);
      int flowers_node = 0;
      std::cin >> flowers_node;

      for (int i = 0; i < length - 1; i++) {
        int node1 = 0;
        int node2 = 0;
        int weight = 0;
        std::cin >> node1 >> node2 >> weight;
        graph.AddEdge(node1, node2, weight);
      }

      std::cout << graph.GetMaxXorViaFlower(flowers_node) << "\n";
    } catch (const std::exception& e) {
      std::cerr << "An exception occurred: " << e.what() << '\n';
      return 1;
    }
  }

  return 0;
}
