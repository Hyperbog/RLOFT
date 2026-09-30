#include "SearchRoutines06C0.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u);
}

std::int16_t readS16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::int16_t>(read16(bytes, offset));
}

} // namespace

void SearchRoutines06C0::updateBounds(SearchState06C0& state) noexcept {
    // 0x06C0..0x06FC. Both source WORDs are sign-extended before subtracting
    // 0x18, and each result is independently clamped to -40.
    std::int32_t x = static_cast<std::int32_t>(state.value255D2) - 0x18;
    std::int32_t y = static_cast<std::int32_t>(state.value255D6) - 0x18;
    if (y < -40) {
        y = -40;
    }
    if (x < -40) {
        x = -40;
    }
    state.value12740 = x;
    state.value12744 = y;
}

std::int32_t SearchRoutines06C0::find0700(SearchState06C0& state,
                                          std::int32_t first,
                                          std::int32_t second,
                                          std::int32_t start) noexcept {
    if (state.alternateTable) {
        for (std::int32_t i = start; i < state.count258A6; ++i) {
            const std::size_t base = static_cast<std::size_t>(i) * 0x16u;
            if (readS16(state.table22230, base + 0x0Eu) == first &&
                readS16(state.table22230, base + 0x0Au) == second) {
                return i;
            }
        }
        return -1;
    }

    for (std::int32_t i = start; i < state.count604C; ++i) {
        const std::size_t base = static_cast<std::size_t>(i) * 0x16u;
        if (readS16(state.table6048, base + 0x06u) == first &&
            readS16(state.table6048, base + 0x02u) == second) {
            return i;
        }
    }
    return -1;
}

std::int32_t SearchRoutines06C0::find0794(const SearchState06C0& state,
                                          std::int32_t first,
                                          std::int32_t second,
                                          std::int32_t start) noexcept {
    if (!state.alternateTable) {
        return -1;
    }

    for (std::int32_t i = start; i < state.count258A4; ++i) {
        const std::size_t base = static_cast<std::size_t>(i) * 0x16u;
        if (readS16(state.table17610, base + 0x0Eu) == first &&
            readS16(state.table17610, base + 0x0Au) == second) {
            return i;
        }
    }
    return -1;
}

std::int32_t SearchRoutines06C0::find07E4(const SearchState06C0& state,
                                          std::int32_t first,
                                          std::int32_t second,
                                          std::int32_t start) noexcept {
    for (std::int32_t i = start; i < state.count258A6; ++i) {
        const std::size_t base = static_cast<std::size_t>(i) * 0x16u;
        if (read16(state.table22230, base + 0x16u) != 0x0054u) {
            continue;
        }
        if (readS16(state.table22230, base + 0x0Eu) == first &&
            readS16(state.table22230, base + 0x0Au) == second) {
            return i;
        }
    }
    return -1;
}

} // namespace ravenloft::game
