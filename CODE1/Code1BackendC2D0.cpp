#include "Code1BackendC2D0.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void Code1BackendC2D0::initialize(
    std::int32_t fileToken,
    std::int32_t arg1,
    std::int32_t arg2,
    std::int32_t arg3,
    std::int32_t arg4,
    Code1BackendStateC2D0& state,
    const Code1BackendOpsC2D0& ops) {
    if (state.active2F48 != 0) {
        return;
    }

    state.dosLinear2F68 =
        ops.allocateDos6F10(
            0x612B0,
            state.dosFar2F6C,
            state.dosSelector2F70);

    if (state.dosLinear2F68 == 0) {
        return;
    }

    const std::int32_t handle =
        ops.openRead62A8(fileToken);

    if (handle == -1) {
        ops.freeDos6F74(state.dosSelector2F70);
        return;
    }

    const std::int32_t size =
        ops.query627C(fileToken);

    const std::uintptr_t image =
        ops.heapAllocate71C8(
            static_cast<std::int32_t>(size));

    if (image == 0) {
        ops.freeDos6F74(state.dosSelector2F70);
        return;
    }

    (void)ops.read6304(
        reinterpret_cast<void*>(image),
        size,
        handle);
    (void)ops.close62F0(handle);

    const std::uintptr_t prepared =
        ops.callF8C4(image, 5, 0);

    if (prepared == 0) {
        ops.freeDos6F74(state.dosSelector2F70);
        ops.heapFree7240(image);
        return;
    }

    ops.heapFree7240(image);
    ops.callE680();

    state.backend2F74 =
        ops.callEA0B(prepared);

    if (state.backend2F74 == -1) {
        ops.freeDos6F74(state.dosSelector2F70);
        ops.callE6EA(0);
        return;
    }

    Code1BackendDescriptorC2D0* descriptor =
        ops.callEAC8(state.backend2F74);

    if (descriptor->type4 != 2) {
        ops.callE6EA(0);
        ops.freeDos6F74(state.dosSelector2F70);
        return;
    }

    if (ops.callEAE7(
            state.backend2F74,
            arg1,
            arg2,
            arg3,
            arg4) == 0) {
        ops.callE6EA(0);
        ops.freeDos6F74(state.dosSelector2F70);
        return;
    }

    ops.callEAF1(
        state.backend2F74,
        arg1,
        arg2,
        arg3,
        arg4);

    state.active2F48 = 1;
}

void Code1BackendC2D0::shutdown(
    Code1BackendStateC2D0& state,
    const Code1BackendOpsC2D0& ops) {
    if (state.active2F48 == 0) {
        return;
    }

    stopIfType2(state, ops);
    ops.callEA9A(state.backend2F74);
    ops.freeDos6F74(state.dosSelector2F70);
    state.active2F48 = 0;
}

void Code1BackendC2D0::submit(
    std::int32_t resourceId,
    Code1BackendStateC2D0& state,
    const Code1BackendOpsC2D0& ops) {
    if (state.active2F48 == 0 ||
        state.enabled2F78 == 0) {
        return;
    }

    const Code1BackendResourceC4E0 resource =
        ops.lookup954C(
            resourceId,
            state.lookupParam2F4C);

    stopIfType2(state, ops);

    ops.copyD9EE(
        reinterpret_cast<std::uint8_t*>(
            state.dosLinear2F68),
        resource.data,
        resource.size);

    ops.callEC25(
        state.backend2F74,
        state.dosLinear2F68,
        0,
        -1);

    ops.callEC1B(
        state.backend2F74,
        state.dosLinear2F68,
        0,
        state.dosFar2F6C,
        -1);

    ops.callEC39(state.backend2F74);
}

void Code1BackendC2D0::stopIfType2(
    Code1BackendStateC2D0& state,
    const Code1BackendOpsC2D0& ops) {
    if (state.active2F48 == 0) {
        return;
    }

    if (ops.callEC2F(state.backend2F74) == 2) {
        ops.callEC43(state.backend2F74);
    }
}

} // namespace ravenloft::game
