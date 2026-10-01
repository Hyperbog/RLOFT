#include "CharacterTriple1F24.h"

#include "UiFlows1CD0.h"

#include <cstdint>

namespace ravenloft::game {

void CharacterTriple1F24::normalize(
    std::array<std::uint8_t, 3>& fields680D,
    std::uint8_t inheritedDL) noexcept {
    auto& field680D = fields680D[0];
    auto& field680E = fields680D[1];
    auto& field680F = fields680D[2];

    // CODE.1 does not initialize DL on entry. Preserve the incoming low byte
    // of EDX exactly; only the MOV DL instructions below overwrite it.
    std::uint8_t selector = inheritedDL;

    // 0x1F28..0x20BA. The assembly uses signed byte loads only for the
    // 0xFF sentinel tests; all remaining comparisons are raw byte equality.
    if (static_cast<std::int8_t>(field680E) == -1) {
        selector = field680D;
    } else if (static_cast<std::int8_t>(field680F) != -1) {
        selector = (field680D == 5u || field680E == 5u || field680F == 5u)
            ? 0x0Du : 0x0Eu;
    } else {
        // Independent blocks match the original sequential MOV DL behavior.
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
        if (field680D == 2u) selector = 0x0Au;
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

    // 0x20BC: MOV AL,DL; SUB AL,6; CMP AL,8; JA 0x2191.
    // This is byte arithmetic: inherited values outside 6..14 simply return.
    const std::uint8_t dispatch = static_cast<std::uint8_t>(selector - 6u);
    if (dispatch > 8u) return;

    // 0x20D5 jumps through the nine-DWORD table at 0x1F00.
    switch (dispatch) {
    case 0u:
    case 1u:
    case 2u:
        if (field680D != 0u) SwapByte1EE4::swap(field680D, field680E);
        return;
    case 3u:
    case 4u:
    case 5u:
        if (field680D != 3u) SwapByte1EE4::swap(field680D, field680E);
        return;
    case 6u:
        if (field680D != 4u) SwapByte1EE4::swap(field680D, field680E);
        return;
    case 7u:
        field680D = 0u;
        field680E = 4u;
        field680F = 5u;
        return;
    case 8u:
        field680D = 0u;
        field680E = 4u;
        field680F = 3u;
        return;
    }
}

} // namespace ravenloft::game
