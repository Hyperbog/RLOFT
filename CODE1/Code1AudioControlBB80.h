#pragma once

#include "Code1AudioPlaybackB76C.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1AudioControlOpsBB80 {
    Code1AudioPlaybackOpsB76C playback;
    std::function<std::int32_t(std::int32_t, std::int32_t)> callED15;
};

class Code1AudioControlBB80 {
public:
    // Direct reconstruction of CODE.1 0xBB80..0xBBE8.
    // CODE.1 0xBBE9..0xBBEB is alignment.
    static void updateFourthParameter(
        std::int32_t value,
        const Code1AudioInitStateB0F8& init,
        Code1AudioPlaybackStateB76C& state,
        const Code1AudioControlOpsBB80& ops);

    // Direct reconstruction of CODE.1 0xBBEC..0xBC19.
    // CODE.1 0xBC1A..0xBC1B is alignment.
    static void restartWithValue(
        std::int32_t channel,
        std::int32_t value,
        const Code1AudioInitStateB0F8& init,
        const Code1SampleDataStateB060& sample,
        Code1AudioChannelsStateB598& channels,
        Code1AudioPlaybackStateB76C& state,
        const Code1AudioChannelsOpsB598& channelOps,
        const Code1AudioControlOpsBB80& ops);

    // Direct reconstruction of CODE.1 0xBC1C..0xBCAF.
    // CODE.1 0xBCB0..0xBCB3 is alignment.
    static void loadAndRestartWithValue(
        std::int32_t resource,
        std::int32_t channel,
        std::int32_t value,
        const Code1AudioInitStateB0F8& init,
        const Code1SampleDataStateB060& sample,
        Code1AudioChannelsStateB598& channels,
        Code1AudioPlaybackStateB76C& state,
        const Code1AudioChannelsOpsB598& channelOps,
        const Code1AudioControlOpsBB80& ops);

    // Direct reconstruction of CODE.1 0xBCB4..0xBD1F.
    static void waitUntilBackendIdle(
        const Code1AudioInitStateB0F8& init,
        const Code1SampleDataStateB060& sample,
        Code1AudioChannelsStateB598& channels,
        Code1AudioPlaybackStateB76C& state,
        const Code1AudioChannelsOpsB598& channelOps,
        const Code1AudioControlOpsBB80& ops);

    // Direct reconstruction of CODE.1 0xBD20..0xBD82.
    // CODE.1 0xBD83 is alignment.
    static void setScaledValue(
        std::int32_t value,
        const Code1AudioInitStateB0F8& init,
        Code1AudioChannelsStateB598& channels,
        Code1AudioPlaybackStateB76C& state,
        const Code1AudioChannelsOpsB598& channelOps,
        const Code1AudioControlOpsBB80& ops);

    // Direct reconstruction of CODE.1 0xBD84..0xBDCB.
    static void setSecondaryValue(
        std::int32_t value,
        const Code1AudioInitStateB0F8& init,
        Code1AudioPlaybackStateB76C& state,
        const Code1AudioControlOpsBB80& ops);

    // Direct reconstruction of CODE.1 0xBDCC..0xBDD7.
    static std::int32_t currentChannel(
        const Code1AudioPlaybackStateB76C& state) noexcept;

    // Direct reconstruction of CODE.1 0xBDD8..0xBDE3.
    static std::int32_t currentResource(
        const Code1AudioPlaybackStateB76C& state) noexcept;

    // Direct reconstruction of CODE.1 0xBDE4..0xBE25.
    // CODE.1 0xBE26..0xBE27 is alignment.
    static std::int32_t stopped(
        const Code1AudioInitStateB0F8& init,
        const Code1AudioPlaybackStateB76C& state,
        const Code1AudioControlOpsBB80& ops);

    // Direct reconstruction of CODE.1 0xBE28..0xBE33.
    static std::int32_t secondaryValue(
        const Code1AudioPlaybackStateB76C& state) noexcept;

    // Direct reconstruction of CODE.1 0xBE34..0xBE3F.
    static std::int32_t scaledValueSetting(
        const Code1AudioChannelsStateB598& channels) noexcept;

    // Direct reconstruction of CODE.1 0xBE40..0xBE60.
    // CODE.1 0xBE61..0xBE6F is zero/data padding.
    static void setEnabled(
        std::uint8_t enabled,
        const Code1AudioInitStateB0F8& init,
        const Code1SampleDataStateB060& sample,
        Code1AudioChannelsStateB598& channels,
        Code1AudioPlaybackStateB76C& state,
        const Code1AudioChannelsOpsB598& channelOps,
        const Code1AudioControlOpsBB80& ops);
};

} // namespace ravenloft::game
