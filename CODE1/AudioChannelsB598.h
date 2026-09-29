#pragma once

#include "AudioInitB0F8.h"
#include "FormatAFD8.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct AudioChannelsStateB598 {
    std::array<std::int32_t, 8> handles2C08{};
    std::array<std::uint8_t, 8> active2C28{};

    std::int32_t value2C3C = 0;

    std::int32_t setting2B80 = 0;
    std::int32_t setting2B88 = 0;
};

struct AudioChannelsOpsB598 {
    std::function<void(
        std::array<std::uint8_t, 0x100>*)> bindConfig8A18;

    std::function<std::int32_t(
        std::int32_t, std::uint32_t, std::int32_t,
        std::uint32_t, std::int32_t)> callEC93;

    std::function<std::uint16_t(
        std::int32_t, std::int32_t)> callECBB;

    std::function<std::uintptr_t(
        std::uint16_t, std::uint16_t)> findSampleB4D4;

    std::function<void(
        std::int32_t, std::uint16_t,
        std::uint16_t, std::uintptr_t)> callECC5;

    std::function<void(
        std::int32_t, std::int32_t)> callEC9D;

    std::function<std::int32_t(
        std::int32_t, std::int32_t)> callED0B;
};

class AudioChannelsB598 {
public:
    // Direct reconstruction of CODE.1 0xB598..0xB5E0.
    // CODE.1 0xB5E1..0xB5E3 is alignment.
    static std::int32_t scaledValue(
        const AudioInitStateB0F8& init,
        AudioChannelsStateB598& state,
        const AudioChannelsOpsB598& ops);

    // Direct reconstruction of CODE.1 0xB5E4..0xB6BD.
    // CODE.1 0xB6BE..0xB6BF is alignment.
    static void start(
        std::int32_t channel,
        const AudioInitStateB0F8& init,
        const SampleDataStateB060& sample,
        AudioChannelsStateB598& state,
        const AudioChannelsOpsB598& ops);

    // Direct reconstruction of CODE.1 0xB6C0..0xB6EA.
    // CODE.1 0xB6EB is alignment.
    static void stop(
        std::int32_t channel,
        const AudioInitStateB0F8& init,
        AudioChannelsStateB598& state,
        const AudioChannelsOpsB598& ops);

    // Direct reconstruction of CODE.1 0xB6EC..0xB74B.
    static void clearAll(
        const AudioInitStateB0F8& init,
        AudioChannelsStateB598& state,
        const AudioChannelsOpsB598& ops);

    // Direct reconstruction of CODE.1 0xB74C..0xB75A.
    // CODE.1 0xB75B is alignment.
    static void setSetting2B80(
        std::int32_t value,
        AudioChannelsStateB598& state) noexcept;

    // Direct reconstruction of CODE.1 0xB75C..0xB76A.
    // CODE.1 0xB76B is alignment.
    static void setSetting2B88(
        std::int32_t value,
        AudioChannelsStateB598& state) noexcept;
};

} // namespace ravenloft::game
