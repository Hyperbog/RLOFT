#include "UiPage064C.h"

#include <bit>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::int32_t add32(std::int32_t lhs, std::uint32_t rhs) noexcept {
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(lhs) + rhs);
}

} // namespace

void UiPage064C::draw(const UiPageState064C& state,
                      const UiPageOps064C& ops) {
    // 0x0651..0x0759: ESI starts at zero and INC/JL visits exactly slots 0..8.
    for (std::int32_t i = 0; i < 9; ++i) {
        const auto index = static_cast<std::size_t>(i);
        const std::int32_t value = state.value8F8[index];
        if (value == -1) {
            continue;
        }

        // 0x0667..0x0696: mode zero gives value 1 its resource-backed path.
        if (state.mode4D0 == 0 && value == 1) {
            ops.call67C8(0, state.x87C[index], state.y880[index],
                         state.resource954[index]);
            continue;
        }

        // 0x069B..0x06CC: slot seven uses DS:0958 unless its value is 0x14.
        if (i == 7 && value != 0x14) {
            ops.call67C8(0, state.x87C[index], state.y880[index],
                         state.specialResource958);
            continue;
        }

        // 0x06D1..0x072D: both comparisons are signed.  The ADD at 0x06EB
        // wraps in 32 bits; do not replace it with potentially-UB signed C++.
        const std::int32_t upper = add32(state.global713C, 0x2Eu);
        if (value >= state.global713C && value < upper) {
            ops.draw.call6C60(0, state.x87C[index], state.y880[index],
                              0x14, 7);
            ops.draw.call6EB0(0,
                              add32(state.x87C[index], 0x15u),
                              state.y880[index],
                              value,
                              1,
                              0x5AF8);
            continue;
        }

        // 0x072F..0x0752: ordinary text/glyph-backed slot.
        ops.draw.call6EB0(0,
                          state.x87C[index],
                          state.y880[index],
                          value,
                          7,
                          0x5FF8);
    }

    // 0x075F..0x079A: 0x004C decodes the selected record into the shared
    // DS:55F8 buffer.  0x00EC consumes that same buffer immediately.
    if (state.textIndex940 != -1) {
        ops.call004C(state.textBuffer55F8, state.textIndex940);
        UiTextComposite00EC::draw(0x114, 0x10C, 0x7A, 0x30,
                                  state.textBuffer55F8, 0, 0x5FF8, ops.draw);
    }

    // 0x079D..0x07D8 repeats the exact decode/draw sequence for DS:0948.
    if (state.textIndex948 != -1) {
        ops.call004C(state.textBuffer55F8, state.textIndex948);
        UiTextComposite00EC::draw(0x114, 0xD7, 0x7A, 0x30,
                                  state.textBuffer55F8, 0, 0x5FF8, ops.draw);
    }

    // 0x07DB..0x07E1 is the epilogue; 0x07E2..0x07E3 is MOV EAX,EAX padding.
}

} // namespace ravenloft::game
