#include "Code1UiPage064C.h"

#include "Code1UiTextComposite00EC.h"

#include <cstddef>

namespace ravenloft::game {

void Code1UiPage064C::draw(const Code1UiPageState064C& state,
                           const Code1TextDatabase& text,
                           const Code1UiPageOps064C& ops) {
    // 0x0651..0x0759: process exactly nine display slots.
    for (std::int32_t i = 0; i < 9; ++i) {
        const auto index = static_cast<std::size_t>(i);
        const std::int32_t value = state.value8F8[index];
        if (value == -1) {
            continue;
        }

        // 0x0667..0x0696.
        if (state.mode4D0 == 0 && value == 1) {
            ops.call67C8(0, state.x87C[index], state.y880[index],
                         state.resource954[index]);
            continue;
        }

        // 0x069B..0x06CC.
        if (i == 7 && value != 0x14) {
            ops.call67C8(0, state.x87C[index], state.y880[index],
                         state.specialResource958);
            continue;
        }

        // 0x06D1..0x072D.
        if (value >= state.global713C && value < state.global713C + 0x2E) {
            ops.draw.call6C60(0, state.x87C[index], state.y880[index],
                              0x14, 7);
            ops.draw.call6EB0(0,
                              state.x87C[index] + 0x15,
                              state.y880[index],
                              value,
                              1,
                              0x5AF8);
            continue;
        }

        // 0x072F..0x0752.
        ops.draw.call6EB0(0,
                          state.x87C[index],
                          state.y880[index],
                          value,
                          7,
                          0x5FF8);
    }

    // 0x075F..0x079A.
    if (state.textIndex940 != -1) {
        const auto& label =
            text.at(static_cast<std::size_t>(state.textIndex940));
        Code1UiTextComposite00EC::draw(0x114, 0x10C, 0x7A, 0x30,
                                       label.c_str(), 0, 0x5FF8, ops.draw);
    }

    // 0x079D..0x07D8.
    if (state.textIndex948 != -1) {
        const auto& label =
            text.at(static_cast<std::size_t>(state.textIndex948));
        Code1UiTextComposite00EC::draw(0x114, 0xD7, 0x7A, 0x30,
                                       label.c_str(), 0, 0x5FF8, ops.draw);
    }
}

} // namespace ravenloft::game
