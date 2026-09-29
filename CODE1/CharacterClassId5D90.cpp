#include "CharacterClassId5D90.h"

namespace ravenloft::game {

std::uint8_t CharacterClassId5D90::classify(
    const std::array<std::uint8_t, 3>& fields680D) noexcept {
    const std::uint8_t first = fields680D[0];
    const std::uint8_t second = fields680D[1];
    const std::uint8_t third = fields680D[2];

    if (static_cast<std::int8_t>(second) == -1) {
        return first;
    }

    if (static_cast<std::int8_t>(third) != -1) {
        return (first == 5u || second == 5u || third == 5u)
            ? 0x0Du
            : 0x0Eu;
    }

    // For valid two-slot states one of the following blocks selects 6..12.
    // Keep the original sequential overwrite order instead of canonicalizing.
    std::uint8_t result = first;

    if (first == 0u) {
        if (second == 5u) result = 6u;
        if (second == 3u) result = 7u;
        if (second == 4u) result = 8u;
    }

    if (first == 5u) {
        if (second == 0u) result = 6u;
        if (second == 3u) result = 9u;
        if (second == 4u) result = 0x0Cu;
    }

    if (first == 2u) {
        result = 0x0Au;
    }

    if (first == 3u) {
        if (second == 0u) result = 7u;
        if (second == 5u) result = 9u;
        if (second == 4u) result = 0x0Bu;
        if (second == 2u) result = 0x0Au;
    }

    if (first == 4u) {
        if (second == 0u) result = 8u;
        if (second == 3u) result = 0x0Bu;
        if (second == 5u) result = 0x0Cu;
    }

    return result;
}

} // namespace ravenloft::game
