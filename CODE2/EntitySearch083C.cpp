#include "EntitySearch083C.h"

#include <cstddef>
#include <cstdlib>

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

std::int32_t EntitySearch083C::find083C(const EntitySearchState083C& state,
                                        std::int32_t first,
                                        std::int32_t second,
                                        std::int32_t start) {
    for (std::int32_t i = start; i < state.count258A2; ++i) {
        const std::size_t base = static_cast<std::size_t>(i) * 0x16u;

        if ((state.records14D78[base + 0x13u] & 0x02u) != 0u) {
            continue;
        }

        const std::uint16_t type = read16(state.records14D78, base + 0x16u);
        if (state.call2ACDC(type) == 0) {
            continue;
        }

        const std::int16_t x = readS16(state.records14D78, base + 0x0Eu);
        const std::int16_t y = readS16(state.records14D78, base + 0x0Au);
        if (x != first || y != second) {
            continue;
        }

        bool accepted = false;
        if (state.valueF03E != 0) {
            accepted = true;
        } else if (state.valueF03C != 0) {
            const std::size_t typeBase = static_cast<std::size_t>(type) * 0x45u;
            accepted = (state.pointedTable22230[typeBase + 0x3Au] & 0x20u) != 0u;
        } else {
            const std::uint16_t flagIndex =
                read16(state.records14D78, base + 0x14u);
            if ((state.flags198E0[static_cast<std::size_t>(flagIndex) * 2u] & 0x40u) != 0u) {
                const std::int32_t dx =
                    static_cast<std::int32_t>(x) - state.value255D2;
                const std::int32_t dy =
                    static_cast<std::int32_t>(y) - state.value255D6;
                accepted = std::abs(dx) <= 10 && std::abs(dy) <= 10;
            }
        }

        if (accepted) {
            return i;
        }
    }

    return -1;
}

std::int32_t EntitySearch083C::find0934(const EntitySearchState083C& state,
                                        std::int32_t first,
                                        std::int32_t second,
                                        std::int32_t start) {
    for (std::int32_t i = start; i < state.count258A2; ++i) {
        const std::size_t base = static_cast<std::size_t>(i) * 0x16u;

        if ((state.records14D78[base + 0x13u] & 0x02u) != 0u) {
            continue;
        }

        const std::uint16_t type = read16(state.records14D78, base + 0x16u);
        if (state.call2AD18(type) == 0 && state.call2ADE0(type) == 0) {
            continue;
        }

        if (readS16(state.records14D78, base + 0x0Eu) == first &&
            readS16(state.records14D78, base + 0x0Au) == second) {
            return i;
        }
    }

    return -1;
}

} // namespace ravenloft::game
