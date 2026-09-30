#include "TileCode09B0.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

std::uint8_t byteAt(const TileCodeState09B0& state,
                    std::int32_t offset) noexcept {
    return state.data[static_cast<std::size_t>(offset)];
}

void inspect(std::uint8_t value,
             std::int32_t bit,
             std::int32_t& lowMask,
             std::int32_t& dotMask) noexcept {
    if (value < 0x20u) {
        lowMask += bit;
    }
    if (value == 0x2Fu || value == 0x2Eu) {
        dotMask += bit;
    }
}

} // namespace

std::int32_t TileCode09B0::run(const TileCodeState09B0& state,
                               std::int32_t base,
                               std::int32_t row,
                               std::int32_t column) noexcept {
    std::int32_t lowMask = 0;
    std::int32_t dotMask = 0;

    if (row != 0) {
        inspect(byteAt(state, base + 0x175F), 1, lowMask, dotMask);
    }

    if (static_cast<std::int32_t>(state.width12756) - 1 > row) {
        inspect(byteAt(state, base + 0x1761), 8, lowMask, dotMask);
    }

    if (column != 0) {
        const std::int32_t p =
            base - static_cast<std::int32_t>(state.width12756) + 0x1760;
        inspect(byteAt(state, p), 2, lowMask, dotMask);
    }

    if (static_cast<std::int32_t>(state.height12754) - 1 > column) {
        const std::int32_t p =
            base + static_cast<std::int32_t>(state.width12756) + 0x1760;
        inspect(byteAt(state, p), 4, lowMask, dotMask);
    }

    const std::uint8_t center = byteAt(state, base + 0x1760);

    if (center == 1u) {
        return static_cast<std::int32_t>(
            state.table5FF0[static_cast<std::size_t>(lowMask)]);
    }

    if (center == 2u) {
        if (lowMask == 6) {
            return 0x29;
        }
        if (lowMask == 9) {
            return 0x2B;
        }
        return lowMask;
    }

    if (center == 3u) {
        if (dotMask == 1) return 0x21;
        if (dotMask == 2) return 0x23;
        if (dotMask == 4) return 0x22;
        if (dotMask == 8) return 0x20;
        if ((dotMask & 0x09) != 0) return 0x20;
        if ((dotMask & 0x06) != 0) return 0x22;
        return lowMask;
    }

    if (center == 4u) {
        if (lowMask == 6) {
            return 0x28;
        }
        if (lowMask == 9) {
            return 0x2A;
        }
        return lowMask;
    }

    if (center == 5u) {
        if ((dotMask & 0x06) == 0x06 ||
            (dotMask & 0x09) == 0x09) {
            return 0x33;
        }
        return static_cast<std::int32_t>(
            state.table5FF0[static_cast<std::size_t>(lowMask)]);
    }

    return lowMask;
}

} // namespace ravenloft::game
