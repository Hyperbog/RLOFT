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
    // 0x2998 reserves exactly 24 bytes: these six DWORDs are the entire local
    // option array at EBP-1C..EBP-08. 0x28E4 initializes all six.
    std::array<std::int32_t, 6> slots{};
    CharacterFlags28E4::build(slots, state.fields680D);

    // 0x29A7..0x29CC converts nonzero flags in-place to resources 0E..13 and
    // zero flags to 15. Arithmetic/indexing is exactly six iterations.
    for (std::uint32_t edx = 0; edx != 6u; ++edx) {
        slots[edx] = slots[edx] != 0
            ? static_cast<std::int32_t>(edx + 0x0Eu)
            : 0x15;
    }

    // 0x29CE..0x29F9 pushes the six local DWORDs plus three literal -1 slots.
    std::array<std::int32_t, 9> menu{
        slots[0], slots[1], slots[2], slots[3], slots[4], slots[5], -1, -1, -1
    };
    ops.call1464(3, menu, 0x0C, -1, 0x29);

    for (;;) {
        // 0x29FE..0x2A03: EDX and ECX receive the full EAX return from 0x043C.
        const std::int32_t selection = ops.call043C();

        if (selection == 0) {
            // 0x2A3C..0x2A52.
            state.mode4D0 = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(state.mode4D0) - 1u);
            ops.call15B0(-1, -1, -1, -1);
            return;
        }

        // 0x2A0B: CMP DWORD [EBP+EAX*4-20],15. There is deliberately no
        // bounds check. EAX 1..6 maps to locals EBP-1C..EBP-08. Everything
        // else is an original-machine frame read supplied by the embedding.
        std::uint32_t compared;
        if (selection >= 1 && selection <= 6) {
            compared = static_cast<std::uint32_t>(
                slots[static_cast<std::size_t>(selection - 1)]);
        } else {
            compared = ops.readUncheckedFrameDword(selection);
        }
        if (compared == 0x15u) continue;

        // 0x2A12..0x2A20 only modifies BL: low byte of ECX minus one.
        state.field6812 = static_cast<std::uint8_t>(
            static_cast<std::uint8_t>(selection) - std::uint8_t{1});

        // INC DWORD DS:04D0 wraps modulo 2^32.
        state.mode4D0 = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(state.mode4D0) + 1u);

        // EDX still holds the full 0x043C return and DEC EDX precedes PUSH.
        const std::int32_t arg0 = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(selection) - 1u);
        ops.call15B0(arg0, 0x114, 0xA2, 4);
        return;
    }
}

} // namespace ravenloft::game
