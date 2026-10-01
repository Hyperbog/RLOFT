#include "CharacterOptions2198.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint32_t scanFlags(const std::array<std::int32_t, 6>& out) noexcept {
    std::uint32_t edx = 0;
    for (std::size_t i = 0; i < 6; ++i) {
        if (out[i] != 0) edx = 1;
    }
    return edx;
}

} // namespace

std::uint32_t CharacterOptions2198::build(
    std::array<std::int32_t, 6>& out,
    std::int32_t mode,
    const CharacterOptionState2198& state,
    std::uint32_t inheritedEdx,
    const CharacterOptionOps2198& ops) noexcept {
    if (mode == 0) {
        // 0x21A2..0x21D0. No bounds check exists in CODE.1.
        for (std::uint32_t eax = 0; eax < 6; ++eax) {
            const std::uint32_t address =
                0x06D8u + static_cast<std::uint32_t>(state.field680C) * 6u + eax;
            out[eax] = static_cast<std::int32_t>(ops.readDsS8(address));
        }
        return scanFlags(out); // 0x21D2..0x21F0
    }

    if (mode == 1) {
        // 0x21FB XOR EDX,EDX occurs before the table loop. The table index uses
        // MOVSX on field680D and otherwise performs unchecked 32-bit arithmetic.
        for (std::uint32_t edx = 0; edx < 6; ++edx) {
            const std::uint32_t classOffset =
                static_cast<std::uint32_t>(state.field680C) * 36u;
            const std::int32_t signedD = static_cast<std::int8_t>(state.field680D);
            const std::uint32_t dOffset =
                static_cast<std::uint32_t>(signedD * 6);
            const std::uint32_t address = 0x06FCu + classOffset + dOffset + edx;
            out[edx] = static_cast<std::int32_t>(ops.readDsS8(address));
        }
        return scanFlags(out); // 0x2244..0x2269
    }

    if (mode != 2) {
        // 0x226E jumps directly to MOV EAX,EDX. Neither CMP modifies EDX.
        return inheritedEdx;
    }

    // 0x2274..0x2281 is memset(out,0,0x18); 0x2284 then explicitly clears EDX.
    out.fill(0);

    const std::uint8_t d = state.field680D;
    const std::uint8_t e = state.field680E;

    if (state.field680C == 5u) {
        if (d == 0u) {
            if (e == 4u) out[5] = 1;
            if (e == 5u) out[4] = 1;
        }
        if (d == 4u) {
            if (e == 0u) out[5] = 1;
            if (e == 5u) out[0] = 1;
        }
        if (d == 5u && e == 4u) out[0] = 1;
    } else if (state.field680C == 1u) {
        if (d == 0u) {
            if (e == 4u) { out[5] = 1; out[3] = 1; }
            if (e == 5u) out[4] = 1;
            if (e == 3u) out[4] = 1;
        }
        if (d == 4u) {
            if (e == 0u) { out[5] = 1; out[3] = 1; }
            if (e == 5u) out[0] = 1;
            if (e == 3u) out[0] = 1;
        }
        if (d == 5u) {
            if (e == 4u) out[0] = 1;
            if (e == 0u) { out[4] = 1; out[3] = 1; }
        }
        if (d == 3u) {
            if (e == 4u) out[0] = 1;
            if (e == 0u) out[4] = 1;
        }
    }

    // 0x24AD..0x24C9 starts with EAX=0 but deliberately retains EDX=0 from
    // 0x2284, setting it to one if any output DWORD is nonzero.
    return scanFlags(out);
}

} // namespace ravenloft::game
