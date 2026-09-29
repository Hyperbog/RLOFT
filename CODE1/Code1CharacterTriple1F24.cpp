#include "Code1CharacterTriple1F24.h"

#include "Code1UiFlows1CD0.h"

#include <cstdint>

namespace ravenloft::game {

void Code1CharacterTriple1F24::normalize(
    std::array<std::uint8_t, 3>& fields680D) noexcept {
    auto& field680D = fields680D[0];
    auto& field680E = fields680D[1];
    auto& field680F = fields680D[2];

    std::uint8_t selector = 0xFFu;

    // 0x1F28..0x20BA. The assembly uses signed byte loads only for the
    // 0xFF sentinel tests; all remaining comparisons are raw byte equality.
    if (static_cast<std::int8_t>(field680E) == -1) {
        selector = field680D;
    } else if (static_cast<std::int8_t>(field680F) != -1) {
        selector = (field680D == 5u || field680E == 5u || field680F == 5u)
            ? 0x0Du : 0x0Eu;
    } else {
        // These tests are deliberately independent rather than else-if chains:
        // the original writes DL sequentially and later matches override earlier
        // ones when an input combination reaches more than one block.
        if (field680D == 0u) {
            if (field680E == 5u) selector = 6u;
            if (field680E == 3u) selector = 7u;
            if (field680E == 4u) selector = 8u;
        }
        if (field680D == 5u) {
            if (field680E == 0u) selector = 6u;
            if (field680E == 3u) selector = 9u;
            if (field680E == 4u) selector = 0x0Cu;
        }
        if (field680D == 2u) {
            selector = 0x0Au;
        }
        if (field680D == 3u) {
            if (field680E == 0u) selector = 7u;
            if (field680E == 5u) selector = 9u;
            if (field680E == 4u) selector = 0x0Bu;
            if (field680E == 2u) selector = 0x0Au;
        }
        if (field680D == 4u) {
            if (field680E == 0u) selector = 8u;
            if (field680E == 3u) selector = 0x0Bu;
            if (field680E == 5u) selector = 0x0Cu;
        }
    }

    // 0x20BC..0x20D5 dispatches selector 6..14 through the nine-entry table
    // stored at CODE.1 0x1F00..0x1F23.
    switch (selector) {
    case 6u:
    case 7u:
    case 8u:
        if (field680D != 0u) {
            Code1SwapByte1EE4::swap(field680D, field680E);
        }
        break;

    case 9u:
    case 0x0Au:
    case 0x0Bu:
        if (field680D != 3u) {
            Code1SwapByte1EE4::swap(field680D, field680E);
        }
        break;

    case 0x0Cu:
        if (field680D != 4u) {
            Code1SwapByte1EE4::swap(field680D, field680E);
        }
        break;

    case 0x0Du:
        field680D = 0u;
        field680E = 4u;
        field680F = 5u;
        break;

    case 0x0Eu:
        field680D = 0u;
        field680E = 4u;
        field680F = 3u;
        break;

    default:
        // Valid shipped states select 6..14. An unmatched state reaches the
        // original routine with no defined jump-table selector; leave it
        // unchanged rather than inventing a transformation.
        break;
    }
}

} // namespace ravenloft::game
