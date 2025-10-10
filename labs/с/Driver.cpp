#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <vector>

#include "ItemFeed.hpp"

namespace youndex::express::test {

void Print(std::ostream& out, std::string_view name, const std::vector<std::uint64_t>& ids) {
  out << name << ": [";
  for (const auto uid : ids) {
    out << uid << ", ";
  }
  out << "]";
}

void TestStdInOut() {
  ItemFeed feed;

  std::size_t actions_count = 0;
  std::cin >> actions_count;
  std::cout << "Started" << "\n";

  for (std::size_t action = 0; action < actions_count; ++action) {
    char command = 0;
    std::cin >> command;

    switch (command) {
      case 'i': {  // i <id> <score>
        std::uint64_t uid = 0;
        std::uint16_t score = 0;
        std::cin >> uid >> score;

        feed.Add({.id = uid, .score = score});
        std::cout << "Added" << "\n";
      } break;
      case 'u': {
        std::uint64_t uid = 0;
        std::uint16_t score = 0;
        std::cin >> uid >> score;

        feed.Update({.id = uid, .score = score});
        std::cout << "Updated" << "\n";
      } break;
      case 'r': {  // r <id>
        std::uint64_t uid = 0;
        std::cin >> uid;

        feed.Remove(uid);
        std::cout << "Removed" << "\n";
      } break;
      case 'p': {  // p <pos> <limit>
        std::uint64_t pos = 0;
        std::size_t limit = 0;
        std::cin >> pos >> limit;

        Print(std::cout, "AtPos", feed.GetAtPosition(pos, limit));
        std::cout << "\n";
      } break;
      case 'g': {  // g <id> <limit>
        std::uint64_t uid = 0;
        std::size_t limit = 0;
        std::cin >> uid >> limit;

        Print(std::cout, "AtId", feed.GetAtItemWithId(uid, limit));
        std::cout << "\n";
      } break;
      default: {
        assert(false);
      } break;
    }
  }
}

void Main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::cerr.tie(nullptr);

  TestStdInOut();
}

}  // namespace youndex::express::test

int main() {
  youndex::express::test::Main();
}