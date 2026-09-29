#include "AudioInitB0F8.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void AudioInitB0F8::initialize(
    const std::string& fileName,
    std::int32_t arg0C,
    std::int32_t arg10,
    std::int32_t arg14,
    std::int32_t arg18,
    AudioInitStateB0F8& state,
    const AudioInitOpsB0F8& ops) {
    // 0xB0FE..0xB105.
    if (state.initialized2B84) {
        return;
    }

    // 0xB10B..0xB129.
    state.mainLinear2B9C =
        ops.allocateDos6F10(
            0xC800,
            state.mainFar2BA0,
            state.mainSelector2BA4);

    if (state.mainLinear2B9C == 0) {
        return;
    }

    // 0xB12F..0xB152.
    const std::int32_t handle =
        ops.openRead62A8(fileName);

    if (handle == -1) {
        (void)ops.freeDos6F74(
            state.mainSelector2BA4);
        return;
    }

    // 0xB157..0xB189.
    const std::int32_t allocationSize =
        ops.querySize627C(fileName);

    const std::uintptr_t raw =
        ops.heapAllocate71C8(
            static_cast<std::uint32_t>(
                allocationSize));

    if (raw == 0) {
        // Preserve original leak: the already-open file handle is not closed
        // on this branch.
        (void)ops.freeDos6F74(
            state.mainSelector2BA4);
        return;
    }

    // 0xB18A..0xB1A7. The original deliberately queries the size again.
    const std::int32_t readSize =
        ops.querySize627C(fileName);

    (void)ops.read6304(
        reinterpret_cast<void*>(raw),
        readSize,
        handle);

    (void)ops.close62F0(handle);

    // 0xB1AA..0xB1DD.
    const std::uintptr_t decoded =
        ops.callF8C4(raw, 5, 0);

    if (decoded == 0) {
        (void)ops.freeDos6F74(
            state.mainSelector2BA4);
        ops.heapFree7240(raw);
        return;
    }

    ops.heapFree7240(raw);

    // 0xB1E7..0xB1F5.
    if (state.backendFlag2F48 == 0) {
        ops.callE680();
    }

    // 0xB1F5..0xB232.
    state.object2B90 =
        ops.callEA0B(decoded);

    if (state.object2B90 == -1) {
        if (state.backendFlag2F48 == 0) {
            (void)ops.callE6EA(0);
        }

        (void)ops.freeDos6F74(
            state.mainSelector2BA4);
        return;
    }

    // 0xB233..0xB26C.
    const auto* descriptor =
        ops.callEAC8(state.object2B90);

    if (descriptor->field4 != 3) {
        if (state.backendFlag2F48 == 0) {
            (void)ops.callE6EA(0);
        }

        (void)ops.freeDos6F74(
            state.mainSelector2BA4);
        return;
    }

    // 0xB26D..0xB2B8.
    if (ops.callEAE7(
            state.object2B90,
            arg0C,
            arg10,
            arg14,
            arg18) == 0) {
        if (state.backendFlag2F48 == 0) {
            (void)ops.callE6EA(0);
        }

        (void)ops.freeDos6F74(
            state.mainSelector2BA4);
        return;
    }

    // 0xB2B9..0xB2D4.
    ops.callEAF1(
        state.object2B90,
        arg0C,
        arg10,
        arg14,
        arg18);

    // 0xB2D7..0xB372.
    state.blockSize2C04 =
        ops.callEC89(state.object2B90);

    if (state.blockSize2C04 != 0) {
        for (std::size_t index = 0;
             index < 8;
             ++index) {
            state.blockLinear2BA8[index] =
                ops.allocateDos6F10(
                    static_cast<std::int32_t>(
                        state.blockSize2C04),
                    state.blockFar2BC8[index],
                    state.blockSelector2BE8[index]);

            if (state.blockLinear2BA8[index] == 0) {
                if (state.backendFlag2F48 == 0) {
                    (void)ops.callE6EA(0);
                }

                for (std::size_t prior = 0;
                     prior < index;
                     ++prior) {
                    (void)ops.freeDos6F74(
                        state.blockSelector2BE8[prior]);
                }

                (void)ops.freeDos6F74(
                    state.mainSelector2BA4);
                return;
            }
        }
    }

    // 0xB372..0xB401.
    state.extraSize2B8C =
        ops.callECA7(state.object2B90);

    if (state.extraSize2B8C != 0) {
        state.extraLinear2BF8 =
            ops.allocateDos6F10(
                static_cast<std::int32_t>(
                    state.extraSize2B8C),
                state.extraFar2BFC,
                state.extraSelector2C00);

        if (state.extraLinear2BF8 == 0) {
            // Preserve the original B3BD cleanup bug: the loop increments
            // EBX, but selector indexing uses EAX instead of EBX.
            std::int32_t eaxIndex = 0;

            if (state.backendFlag2F48 == 0) {
                eaxIndex =
                    ops.callE6EA(0);
            }

            for (std::int32_t ebx = 0;
                 ebx < 8;
                 ++ebx) {
                (void)ebx;
                (void)ops.freeDos6F74(
                    state.blockSelector2BE8[
                        static_cast<std::size_t>(
                            eaxIndex)]);
            }

            (void)ops.freeDos6F74(
                state.mainSelector2BA4);
            return;
        }

        // 0xB3EC..0xB3FE.
        ops.callECB1(
            state.object2B90,
            state.extraLinear2BF8,
            state.extraSize2B8C);
    }

    // 0xB401..0xB419.
    ops.bindConfig8A18(
        state.config2AA0);

    const std::uint16_t word32 =
        static_cast<std::uint16_t>(
            (*state.config2AA0)[0x32]) |
        (static_cast<std::uint16_t>(
             (*state.config2AA0)[0x33]) << 8u);

    if (word32 != 0 &&
        word32 != 3) {
        ops.loadSampleB060();
    }

    // 0xB41E.
    state.initialized2B84 = true;
}

} // namespace ravenloft::game
