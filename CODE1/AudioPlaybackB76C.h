#pragma once

#include "AudioChannelsB598.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct AudioPlaybackStateB76C {
    std::uint8_t enabled2C44 = 0;

    std::int32_t currentHandle2C30 = -1;
    std::int32_t currentResource2C34 = -1;
    std::int32_t currentChannel2C38 = -1;
    std::int32_t value2C40 = 0;
};

struct AudioPlaybackOpsB76C {
    std::function<void(
        std::int32_t, std::int32_t,
        std::int32_t, std::int32_t)> callED29;

    std::function<void(
        std::int32_t, std::int32_t,
        std::int32_t, std::int32_t)> callED33;

    std::function<void(
        std::int32_t, std::int32_t)> callECED;

    std::function<void(
        std::int32_t, std::int32_t)> callED01;

    std::function<void(
        std::int32_t, std::int32_t)> callECF7;

    std::function<std::int32_t(
        std::int32_t, std::int32_t)> callED0B;

    std::function<std::uintptr_t(
        std::int32_t, std::int32_t)> ensureResource954C;

    std::function<void(
        std::uint32_t, const void*, std::uint32_t)> callD1E8;
};

class AudioPlaybackB76C {
public:
    // Direct reconstruction of CODE.1 0xB76C..0xB820.
    // CODE.1 0xB821..0xB823 is alignment.
    static void select(
        std::int32_t channel,
        const AudioInitStateB0F8& init,
        const SampleDataStateB060& sample,
        AudioChannelsStateB598& channels,
        AudioPlaybackStateB76C& state,
        const AudioChannelsOpsB598& channelOps,
        const AudioPlaybackOpsB76C& ops);

    // Direct reconstruction of CODE.1 0xB824..0xB8F2.
    // CODE.1 0xB8F3 is alignment.
    static void selectWithValue(
        std::int32_t channel,
        std::int32_t ignoredValue,
        const AudioInitStateB0F8& init,
        const SampleDataStateB060& sample,
        AudioChannelsStateB598& channels,
        AudioPlaybackStateB76C& state,
        const AudioChannelsOpsB598& channelOps,
        const AudioPlaybackOpsB76C& ops);

    // Direct reconstruction of CODE.1 0xB8F4..0xBA00.
    // CODE.1 0xBA01..0xBA03 is alignment.
    static void modify(
        std::int32_t channel,
        std::int32_t branchValue,
        const AudioInitStateB0F8& init,
        const SampleDataStateB060& sample,
        AudioChannelsStateB598& channels,
        AudioPlaybackStateB76C& state,
        const AudioChannelsOpsB598& channelOps,
        const AudioPlaybackOpsB76C& ops);

    // Direct reconstruction of CODE.1 0xBA04..0xBA96.
    // CODE.1 0xBA97 is alignment.
    static void loadAndSelect(
        std::int32_t resource,
        std::int32_t channel,
        const AudioInitStateB0F8& init,
        const SampleDataStateB060& sample,
        AudioChannelsStateB598& channels,
        AudioPlaybackStateB76C& state,
        const AudioChannelsOpsB598& channelOps,
        const AudioPlaybackOpsB76C& ops);

    // Direct reconstruction of CODE.1 0xBA98..0xBB7F.
    static void stopCurrent(
        const AudioInitStateB0F8& init,
        const SampleDataStateB060& sample,
        AudioChannelsStateB598& channels,
        AudioPlaybackStateB76C& state,
        const AudioChannelsOpsB598& channelOps,
        const AudioPlaybackOpsB76C& ops);
};

} // namespace ravenloft::game
