#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace youndex::express {

    struct Item {
        std::uint64_t id;
        std::uint16_t score;
    };

    /// NB: Is not thread safe.
    class ItemFeed final {
    public:
        [[nodiscard]] std::vector<std::uint64_t> GetAtPosition(std::size_t position, std::size_t limit)
            const {
            assert(position < Size());
            assert(1 <= limit && limit <= 16);

            (void)position;
            (void)limit;

            std::abort();  // Not implemented
        }

        [[nodiscard]] std::vector<std::uint64_t> GetAtItemWithId(std::uint64_t uid, std::size_t limit)
            const {
            assert(Contains(uid));
            assert(1 <= limit && limit <= 16);

            (void)uid;
            (void)limit;

            std::abort();  // Not implemented
        }

        void Add(Item item) {
            assert(!Contains(item.id));

            (void)item;

            std::abort();  // Not implemented
        }

        void Update(Item item) {
            assert(Contains(item.id));

            (void)item;

            std::abort();  // Not implemented
        }

        void Remove(std::uint64_t uid) {
            assert(Contains(uid));

            (void)uid;

            std::abort();  // Not implemented
        }

    private:
        [[nodiscard]] bool Contains(std::uint64_t uid) const {
            assert(1 <= uid);

            (void)uid;

            std::abort();  // Not implemented
        }

        [[nodiscard]] std::size_t Size() const {
            std::abort();  // Not implemented
        }
    };

}  // namespace youndex::express