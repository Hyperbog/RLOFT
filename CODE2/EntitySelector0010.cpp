#include "EntitySelector0010.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u);
}

void write16(std::span<std::uint8_t> bytes,
             std::size_t offset,
             std::uint16_t value) noexcept {
    bytes[offset] = static_cast<std::uint8_t>(value & 0xFFu);
    bytes[offset + 1u] = static_cast<std::uint8_t>(value >> 8u);
}

} // namespace

void EntitySelector0010::run(EntitySelectorState0010& state,
                             std::int32_t first,
                             std::int32_t second) noexcept {
    // EDX starts at zero and is compared signed against the WORD at 0x258A6.
    for (std::int32_t index = 0; index < state.count258A6; ++index) {
        const std::size_t base = static_cast<std::size_t>(index) * 0x16u;

        // 0x002C..0x0036.
        if (read16(state.table22230, base + 0x16u) != 0x0054u) {
            continue;
        }

        // 0x0038..0x004C: MOVSX makes both table fields signed 16-bit values.
        if (static_cast<std::int16_t>(read16(state.table22230, base + 0x0Eu)) != first ||
            static_cast<std::int16_t>(read16(state.table22230, base + 0x0Au)) != second) {
            continue;
        }

        // 0x004E..0x0057: AND WORD,0xFE3F then OR low BYTE,0x40.
        const std::uint16_t flags = read16(state.table22230, base + 0x1Cu);
        write16(state.table22230,
                base + 0x1Cu,
                static_cast<std::uint16_t>((flags & 0xFE3Fu) | 0x0040u));
        return;
    }
}

} // namespace ravenloft::game
