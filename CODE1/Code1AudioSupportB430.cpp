#include "Code1AudioSupportB430.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint32_t read32(
    const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8u) |
           (static_cast<std::uint32_t>(p[2]) << 16u) |
           (static_cast<std::uint32_t>(p[3]) << 24u);
}

void write32(
    std::uint8_t* p,
    std::uint32_t value) noexcept {
    p[0] = static_cast<std::uint8_t>(value);
    p[1] = static_cast<std::uint8_t>(value >> 8u);
    p[2] = static_cast<std::uint8_t>(value >> 16u);
    p[3] = static_cast<std::uint8_t>(value >> 24u);
}

} // namespace

void Code1AudioSupportB430::shutdown(
    Code1AudioInitStateB0F8& state,
    Code1SampleDataStateB060& sample,
    const Code1AudioSupportOpsB430& ops) {
    if (!state.initialized2B84) {
        return;
    }

    // Exact original shutdown order.
    ops.callBA98();
    ops.callB6EC();
    ops.callEA9A(state.object2B90);

    if (state.extraSize2B8C != 0) {
        (void)ops.freeDos6F74(
            state.extraSelector2C00);
    }

    if (state.blockSize2C04 != 0) {
        for (std::size_t index = 0;
             index < 8;
             ++index) {
            (void)ops.freeDos6F74(
                state.blockSelector2BE8[index]);
        }
    }

    (void)ops.freeDos6F74(
        state.mainSelector2BA4);

    if (sample.loaded2B94 != 0) {
        ops.heapFree7240(
            sample.sampleData2B98);
        sample.loaded2B94 = 0;
    }

    state.initialized2B84 = false;
}

std::uintptr_t Code1AudioSupportB430::findSample(
    std::uint16_t first,
    std::uint16_t second,
    const Code1SampleDataStateB060& sample) noexcept {
    const auto* base =
        reinterpret_cast<const std::uint8_t*>(
            sample.sampleData2B98);

    std::uint32_t index = 0;

    for (;;) {
        const auto* record =
            base + index * 6u;

        const std::int32_t firstTag =
            static_cast<std::int8_t>(
                record[1]);

        if (firstTag == -1) {
            return 0;
        }

        if (firstTag ==
                static_cast<std::int32_t>(first) &&
            static_cast<std::int8_t>(
                record[0]) ==
                static_cast<std::int32_t>(second)) {
            return sample.sampleData2B98 +
                read32(record + 2);
        }

        ++index;
    }
}

std::uintptr_t Code1AudioSupportB430::loadBlob(
    std::int32_t handle,
    const Code1AudioInitStateB0F8& state,
    const Code1AudioSupportOpsB430& ops) {
    if (!state.initialized2B84) {
        return 0;
    }

    std::uint32_t length = 0;

    (void)ops.read6304(
        &length,
        4,
        handle);

    const std::uintptr_t resource =
        ops.heapAllocate71C8(
            static_cast<std::size_t>(length) + 4u);

    write32(
        reinterpret_cast<std::uint8_t*>(
            resource),
        length);

    (void)ops.read6304(
        reinterpret_cast<void*>(
            resource + 4u),
        static_cast<std::int32_t>(length),
        handle);

    return resource;
}

void Code1AudioSupportB430::releaseBlob(
    std::uintptr_t resource,
    const Code1AudioInitStateB0F8& state,
    const Code1AudioSupportOpsB430& ops) {
    if (state.initialized2B84 &&
        resource != 0) {
        ops.heapFree7240(resource);
    }
}

} // namespace ravenloft::game
