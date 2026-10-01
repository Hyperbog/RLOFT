#include "CharacterFlags28E4.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void CharacterFlags28E4::build(
    std::array<std::int32_t, 6>& out,
    const std::array<std::uint8_t, 3>& fields680D) noexcept {
    out.fill(1);

    if (fields680D[0] == 1u) {
        // 0x2912..0x2928: memset 24 bytes to zero, then restore DWORD 0 to 1.
        out.fill(0);
        out[0] = 1;
        return;
    }

    if (fields680D[0] == 5u || fields680D[1] == 5u || fields680D[2] == 5u) {
        out[0] = 0;
    }

    if (fields680D[0] == 2u || fields680D[1] == 2u) {
        out[1] = 0;
        out[3] = 0;
        out[5] = 0;
    }
}

void CharacterFlagChoice2994::run(
    CharacterFlagChoiceState2994& state,
    const CharacterFlagChoiceOps2994& ops) {
    std::array<std::int32_t, 6> flags{};
    CharacterFlags28E4::build(flags, state.fields680D);

    std::array<std::int32_t, 9> slots{
        -1,-1,-1,-1,-1,-1,-1,-1,-1
    };
    for (std::size_t i = 0; i < 6; ++i) {
        slots[i] = flags[i] != 0 ? static_cast<std::int32_t>(i) + 0x0E : 0x15;
    }

    ops.call1464(3, slots, 0x0C, -1, 0x29);

    for (;;) {
        const std::int32_t selection = ops.call043C();
        if (selection == 0) {
            // DEC DWORD DS:04D0: preserve 32-bit wrap rather than signed C++ UB.
            state.mode4D0 = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(state.mode4D0) - 1u);
            ops.call15B0(-1, -1, -1, -1);
            return;
        }

        // 0x2A0B indexes [EBP+EAX*4-20] directly. CODE.1 has no 1..6 or
        // 0xFF guard here; do not invent one. Shipped hit-test values address
        // the six option DWORDs, while any other EAX retains original raw-stack
        // indexing semantics rather than a sanitized rejection path.
        const auto rawIndex = static_cast<std::uint32_t>(selection) - 1u;
        if (*(slots.data() + rawIndex) == 0x15) continue;

        state.field6812 = static_cast<std::uint8_t>(selection - 1);
        state.mode4D0 = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(state.mode4D0) + 1u);
        ops.call15B0(selection - 1, 0x114, 0xA2, 4);
        return;
    }
}

} // namespace ravenloft::game
