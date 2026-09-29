#pragma once

#include "AudioInitB0F8.h"
#include "FormatAFD8.h"

#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct AudioSupportOpsB430 {
    std::function<void()> callBA98;
    std::function<void()> callB6EC;
    std::function<void(std::int32_t)> callEA9A;

    std::function<std::int32_t(std::uint16_t)> freeDos6F74;
    std::function<void(std::uintptr_t)> heapFree7240;

    std::function<std::int32_t(
        void*, std::int32_t, std::int32_t)> read6304;
    std::function<std::uintptr_t(std::size_t)> heapAllocate71C8;
};

class AudioSupportB430 {
public:
    // Direct reconstruction of CODE.1 0xB430..0xB4D1.
    // CODE.1 0xB4D2..0xB4D3 is alignment.
    static void shutdown(
        AudioInitStateB0F8& state,
        SampleDataStateB060& sample,
        const AudioSupportOpsB430& ops);

    // Direct reconstruction of CODE.1 0xB4D4..0xB51A.
    // CODE.1 0xB51B is alignment.
    static std::uintptr_t findSample(
        std::uint16_t first,
        std::uint16_t second,
        const SampleDataStateB060& sample) noexcept;

    // Direct reconstruction of CODE.1 0xB51C..0xB573.
    // The original first argument is unused; the second argument is the
    // file handle from the resource loader dispatch.
    static std::uintptr_t loadBlob(
        std::int32_t handle,
        const AudioInitStateB0F8& state,
        const AudioSupportOpsB430& ops);

    // Direct reconstruction of CODE.1 0xB574..0xB595.
    // CODE.1 0xB596..0xB597 is alignment.
    static void releaseBlob(
        std::uintptr_t resource,
        const AudioInitStateB0F8& state,
        const AudioSupportOpsB430& ops);
};

} // namespace ravenloft::game
