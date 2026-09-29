#include "AudioControlBB80.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint16_t configWord32(
    const AudioInitStateB0F8& init) noexcept {
    return static_cast<std::uint16_t>(
        (*init.config2AA0)[0x32]) |
        (static_cast<std::uint16_t>(
             (*init.config2AA0)[0x33]) << 8u);
}

} // namespace

void AudioControlBB80::updateFourthParameter(
    std::int32_t value,
    const AudioInitStateB0F8& init,
    AudioPlaybackStateB76C& state,
    const AudioControlOpsBB80& ops) {
    if (!init.initialized2B84 ||
        state.currentChannel2C38 == -1) {
        return;
    }

    const std::int32_t status =
        ops.playback.callED0B(
            init.object2B90,
            state.currentHandle2C30);

    if (status == 0 || status == 2) {
        state.currentHandle2C30 = -1;
        state.currentChannel2C38 = -1;
        return;
    }

    // Original push order is:
    // caller value, 0, current handle, backend object.
    ops.playback.callED29(
        init.object2B90,
        state.currentHandle2C30,
        0,
        value);
}

void AudioControlBB80::restartWithValue(
    std::int32_t channel,
    std::int32_t value,
    const AudioInitStateB0F8& init,
    const SampleDataStateB060& sample,
    AudioChannelsStateB598& channels,
    AudioPlaybackStateB76C& state,
    const AudioChannelsOpsB598& channelOps,
    const AudioControlOpsBB80& ops) {
    if (!init.initialized2B84 ||
        state.enabled2C44 == 0) {
        return;
    }

    AudioPlaybackB76C::stopCurrent(
        init,
        sample,
        channels,
        state,
        channelOps,
        ops.playback);

    AudioPlaybackB76C::selectWithValue(
        channel,
        value,
        init,
        sample,
        channels,
        state,
        channelOps,
        ops.playback);
}

void AudioControlBB80::loadAndRestartWithValue(
    std::int32_t resource,
    std::int32_t channel,
    std::int32_t value,
    const AudioInitStateB0F8& init,
    const SampleDataStateB060& sample,
    AudioChannelsStateB598& channels,
    AudioPlaybackStateB76C& state,
    const AudioChannelsOpsB598& channelOps,
    const AudioControlOpsBB80& ops) {
    if (!init.initialized2B84 ||
        state.enabled2C44 == 0) {
        return;
    }

    AudioPlaybackB76C::stopCurrent(
        init,
        sample,
        channels,
        state,
        channelOps,
        ops.playback);

    AudioChannelsB598::clearAll(
        init,
        channels,
        channelOps);

    if (state.currentResource2C34 != resource) {
        std::int32_t item = resource;

        const std::uint16_t mode =
            configWord32(init);

        if (mode == 3 || mode == 4) {
            item += channels.setting2B88;
        }

        const std::uintptr_t loaded =
            ops.playback.ensureResource954C(
                item,
                channels.setting2B80);

        const auto* bytes =
            reinterpret_cast<const std::uint8_t*>(
                loaded);

        const std::uint32_t length =
            static_cast<std::uint32_t>(bytes[0]) |
            (static_cast<std::uint32_t>(bytes[1]) << 8u) |
            (static_cast<std::uint32_t>(bytes[2]) << 16u) |
            (static_cast<std::uint32_t>(bytes[3]) << 24u);

        ops.playback.callD1E8(
            init.mainLinear2B9C,
            bytes + 4,
            length);

        state.currentResource2C34 = resource;
    }

    AudioPlaybackB76C::selectWithValue(
        channel,
        value,
        init,
        sample,
        channels,
        state,
        channelOps,
        ops.playback);
}

void AudioControlBB80::waitUntilBackendIdle(
    const AudioInitStateB0F8& init,
    const SampleDataStateB060& sample,
    AudioChannelsStateB598& channels,
    AudioPlaybackStateB76C& state,
    const AudioChannelsOpsB598& channelOps,
    const AudioControlOpsBB80& ops) {
    if (!init.initialized2B84 ||
        state.currentChannel2C38 == -1) {
        return;
    }

    const std::int32_t status =
        ops.playback.callED0B(
            init.object2B90,
            state.currentHandle2C30);

    if (status == 0 || status == 2) {
        state.currentChannel2C38 = -1;
        state.currentHandle2C30 = -1;
        return;
    }

    while (ops.callED15(
               init.object2B90,
               state.currentHandle2C30) != 0) {
    }

    AudioPlaybackB76C::stopCurrent(
        init,
        sample,
        channels,
        state,
        channelOps,
        ops.playback);
}

void AudioControlBB80::setScaledValue(
    std::int32_t value,
    const AudioInitStateB0F8& init,
    AudioChannelsStateB598& channels,
    AudioPlaybackStateB76C& state,
    const AudioChannelsOpsB598& channelOps,
    const AudioControlOpsBB80& ops) {
    if (channels.value2C3C == value) {
        return;
    }

    channels.value2C3C = value;

    if (!init.initialized2B84 ||
        state.currentChannel2C38 == -1) {
        return;
    }

    if (ops.callED15(
            init.object2B90,
            state.currentHandle2C30) != 0x64) {
        return;
    }

    // The zero pushed at BD61 is ignored by B598, exactly as in the
    // previously reconstructed callers.
    const std::int32_t scaled =
        AudioChannelsB598::scaledValue(
            init,
            channels,
            channelOps);

    ops.playback.callED29(
        init.object2B90,
        state.currentHandle2C30,
        scaled,
        0);
}

void AudioControlBB80::setSecondaryValue(
    std::int32_t value,
    const AudioInitStateB0F8& init,
    AudioPlaybackStateB76C& state,
    const AudioControlOpsBB80& ops) {
    if (state.value2C40 == value) {
        return;
    }

    state.value2C40 = value;

    if (!init.initialized2B84 ||
        state.currentChannel2C38 == -1) {
        return;
    }

    ops.playback.callED33(
        init.object2B90,
        state.currentHandle2C30,
        value,
        0);
}

std::int32_t AudioControlBB80::currentChannel(
    const AudioPlaybackStateB76C& state) noexcept {
    return state.currentChannel2C38;
}

std::int32_t AudioControlBB80::currentResource(
    const AudioPlaybackStateB76C& state) noexcept {
    return state.currentResource2C34;
}

std::int32_t AudioControlBB80::stopped(
    const AudioInitStateB0F8& init,
    const AudioPlaybackStateB76C& state,
    const AudioControlOpsBB80& ops) {
    if (!init.initialized2B84 ||
        state.currentChannel2C38 == -1) {
        return 0;
    }

    const std::int32_t status =
        ops.playback.callED0B(
            init.object2B90,
            state.currentHandle2C30);

    return status == 0 || status == 2
        ? 1
        : 0;
}

std::int32_t AudioControlBB80::secondaryValue(
    const AudioPlaybackStateB76C& state) noexcept {
    return state.value2C40;
}

std::int32_t AudioControlBB80::scaledValueSetting(
    const AudioChannelsStateB598& channels) noexcept {
    return channels.value2C3C;
}

void AudioControlBB80::setEnabled(
    std::uint8_t enabled,
    const AudioInitStateB0F8& init,
    const SampleDataStateB060& sample,
    AudioChannelsStateB598& channels,
    AudioPlaybackStateB76C& state,
    const AudioChannelsOpsB598& channelOps,
    const AudioControlOpsBB80& ops) {
    state.enabled2C44 = enabled;

    if (init.initialized2B84 &&
        enabled == 0) {
        AudioPlaybackB76C::stopCurrent(
            init,
            sample,
            channels,
            state,
            channelOps,
            ops.playback);
    }
}

} // namespace ravenloft::game
