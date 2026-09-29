#include "Code1UiSequence45FC.h"

#include "Code1StateStep44E4.h"

#include <array>
#include <cstddef>

namespace ravenloft::game {

void Code1UiSequence45FC::run(
    Code1UiSequenceState45FC& state,
    const Code1UiSequenceOps45FC& ops) {
    static constexpr std::array<std::uint8_t, 10> kSequence{{
        0x36, 0x37, 0x38, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x3A
    }};

    std::int32_t value = 0;
    std::int32_t delta = 2;

    // 0x461F..0x466D.
    ops.call6588(state.buffer7138);
    ops.call86C4(0, 0, 0x1B, 7);
    ops.call86C4(0xC8, 0, 0x1C, 7);
    const std::uintptr_t savedPanel =
        ops.call7868(0xE5, 0, 0xAA, 0x50, 0xFF);

    // 0x4672..0x46B3.
    Code1UiDrawState4520::draw(value, delta, ops.drawState);
    ops.call6C60(0, 0x15F, 0x74, 0x31, 7);
    ops.callA390(static_cast<std::uint8_t>(state.buffer7138 ^ 1u));
    ops.call65A4(state.buffer7138);
    state.buffer7138 ^= 1u;

    // 0x46BA..0x46CE.
    for (std::int32_t resource = 0x31; resource <= 0x3D; ++resource) {
        (void)ops.call954C(resource, 7);
    }

    // 0x46D0..0x4715.
    ops.call6588(state.buffer7138);
    ops.call67C8(0, 0xE5, 0, savedPanel);

    std::uintptr_t animatedPanel =
        ops.call7868(0x96, 0x8C, 0xE2, 0x68, 0xFF);

    // ESI in the original: two complete animation passes.
    for (std::int32_t pass = 0; pass < 2; ++pass) {
        // 0x4719..0x481A: ten frames, each embedded-table resource held for
        // two frames.
        for (std::int32_t frame = 0; frame < 10; ++frame) {
            ops.call87C4();
            ops.call6588(state.buffer7138);
            ops.call67C8(0, 0x96, 0x8C, animatedPanel);

            const std::size_t sequenceIndex =
                static_cast<std::size_t>(pass * 5 + frame / 2);
            ops.call6C60(0, 0x96, 0x8C, kSequence[sequenceIndex], 7);

            ops.call67C8(0, 0xE5, 0, savedPanel);
            Code1UiDrawState4520::draw(value, delta, ops.drawState);

            ops.call65A4(state.buffer7138);
            state.buffer7138 ^= 1u;
            ops.call87D8(2, 0);

            if (pass == 0 && frame == 0) {
                ops.callBC1C(0x18, 1, 0x9C4);
                ops.callBFE0(0x64F8, 0, 0xFF, 0, 0, 0);
            }

            if (frame == 1) {
                ops.callC4E0(state.value712C + 0x0D);
            }

            Code1StateStep44E4::apply(value, delta);
        }

        // 0x4820..0x48F1: 25-frame second phase.
        for (std::int32_t frame = 0; frame < 25; ++frame) {
            ops.call87C4();
            ops.call6588(state.buffer7138);
            ops.call67C8(0, 0x96, 0x8C, animatedPanel);

            if (pass == 0) {
                ops.call6C60(0, 0x15F, 0x95, 0x32, 7);
            } else {
                ops.call6C60(0, 0x15F, 0xB2, 0x33, 7);
            }

            ops.call67C8(0, 0xE5, 0, savedPanel);
            Code1UiDrawState4520::draw(value, delta, ops.drawState);

            ops.call65A4(state.buffer7138);
            state.buffer7138 ^= 1u;
            ops.call87D8(2, 0);

            Code1StateStep44E4::apply(value, delta);

            if (pass == 0 && frame == 15) {
                ops.callC4E0(state.value712C + 0x0C);
            }
        }

        // 0x48F7..0x4924.
        ops.call7830(animatedPanel);
        animatedPanel =
            ops.call7868(0x96, 0x8C, 0xD8, 0x68, 0xFF);
    }

    // 0x492A..0x4976: release cached resources and captured images.
    ops.call91D8(0x32, 7);
    ops.call91D8(0x33, 7);
    for (std::int32_t resource = 0x34; resource <= 0x3A; ++resource) {
        ops.call91D8(resource, 7);
    }

    ops.call7830(animatedPanel);
    ops.call7830(savedPanel);
}

} // namespace ravenloft::game
