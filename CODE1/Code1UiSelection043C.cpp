#include "Code1UiSelection043C.h"

namespace ravenloft::game {

std::int32_t Code1UiSelection043C::waitForSelection(
    const Code1UiHitTestState035C& hitState,
    std::int32_t& lastButtons4D4,
    const PointerPoll& pollPointer,
    const KeyPoll& pollKey) {
    // 0x0440..0x0447: consume the current press and wait for release.
    while (pollPointer().buttons != 0) {
    }

    for (;;) {
        // 0x0449..0x0450: wait for the next pointer press.
        const auto pointer = pollPointer();
        if (pointer.buttons == 0) {
            // 0x0452..0x047F: only mode 6 checks keyboard while idle.
            if (hitState.mode4D0 != 6) {
                continue;
            }

            // The assembly calls 0x831C independently for both comparisons.
            if (pollKey() == 0x14E) {
                return 7;
            }
            if (pollKey() == 0x14A) {
                return 9;
            }
            continue;
        }

        // 0x0481..0x0496: save the current button mask to DS:04D4.
        lastButtons4D4 = pointer.buttons;

        // 0x049C..0x04A8: keep polling until the hit-test is nonnegative.
        const auto selection =
            Code1UiHitTest035C::hitTest(pointer.x, pointer.y, hitState);
        if (selection >= 0) {
            return selection;
        }
    }
}

} // namespace ravenloft::game
