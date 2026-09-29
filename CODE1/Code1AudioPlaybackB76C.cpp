#include "Code1AudioPlaybackB76C.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint16_t configWord32(
    const Code1AudioInitStateB0F8& init) noexcept {
    return static_cast<std::uint16_t>(
        (*init.config2AA0)[0x32]) |
        (static_cast<std::uint16_t>(
             (*init.config2AA0)[0x33]) << 8u);
}

} // namespace

void Code1AudioPlaybackB76C::select(
    std::int32_t channel,
    const Code1AudioInitStateB0F8& init,
    const Code1SampleDataStateB060& sample,
    Code1AudioChannelsStateB598& channels,
    Code1AudioPlaybackStateB76C& state,
    const Code1AudioChannelsOpsB598& channelOps,
    const Code1AudioPlaybackOpsB76C& ops) {
    if (!init.initialized2B84 ||
        state.enabled2C44 == 0 ||
        channel == state.currentChannel2C38) {
        return;
    }

    stopCurrent(
        init, sample, channels, state,
        channelOps, ops);

    Code1AudioChannelsB598::start(
        channel, init, sample, channels, channelOps);

    state.currentHandle2C30 =
        channels.handles2C08[
            static_cast<std::size_t>(channel)];
    state.currentChannel2C38 = channel;

    if (state.currentHandle2C30 == -1 ||
        channel == -1) {
        return;
    }

    // B598 ignores the stack argument pushed by the caller and reads only
    // global 2C3C. Preserve that behavior.
    const std::int32_t scaled =
        Code1AudioChannelsB598::scaledValue(
            init, channels, channelOps);

    ops.callED29(
        init.object2B90,
        state.currentHandle2C30,
        scaled,
        0);

    ops.callED33(
        init.object2B90,
        state.currentHandle2C30,
        state.value2C40,
        0);

    ops.callECED(
        init.object2B90,
        state.currentHandle2C30);
}

void Code1AudioPlaybackB76C::selectWithValue(
    std::int32_t channel,
    std::int32_t ignoredValue,
    const Code1AudioInitStateB0F8& init,
    const Code1SampleDataStateB060& sample,
    Code1AudioChannelsStateB598& channels,
    Code1AudioPlaybackStateB76C& state,
    const Code1AudioChannelsOpsB598& channelOps,
    const Code1AudioPlaybackOpsB76C& ops) {
    // The original pushes this value to B598, but B598 never reads it.
    (void)ignoredValue;

    if (!init.initialized2B84 ||
        state.enabled2C44 == 0 ||
        channel == state.currentChannel2C38) {
        return;
    }

    stopCurrent(
        init, sample, channels, state,
        channelOps, ops);

    Code1AudioChannelsB598::start(
        channel, init, sample, channels, channelOps);

    state.currentHandle2C30 =
        channels.handles2C08[
            static_cast<std::size_t>(channel)];
    state.currentChannel2C38 = channel;

    if (state.currentHandle2C30 == -1 ||
        channel == -1) {
        return;
    }

    ops.callED33(
        init.object2B90,
        state.currentHandle2C30,
        state.value2C40,
        0);

    ops.callECED(
        init.object2B90,
        state.currentHandle2C30);

    ops.callED29(
        init.object2B90,
        state.currentHandle2C30,
        0,
        0);

    const std::int32_t scaled =
        Code1AudioChannelsB598::scaledValue(
            init, channels, channelOps);

    ops.callED29(
        init.object2B90,
        state.currentHandle2C30,
        scaled,
        0);
}

void Code1AudioPlaybackB76C::modify(
    std::int32_t channel,
    std::int32_t branchValue,
    const Code1AudioInitStateB0F8& init,
    const Code1SampleDataStateB060& sample,
    Code1AudioChannelsStateB598& channels,
    Code1AudioPlaybackStateB76C& state,
    const Code1AudioChannelsOpsB598& channelOps,
    const Code1AudioPlaybackOpsB76C& ops) {
    if (!init.initialized2B84 ||
        state.enabled2C44 == 0 ||
        channel == state.currentChannel2C38) {
        return;
    }

    stopCurrent(
        init, sample, channels, state,
        channelOps, ops);

    state.currentHandle2C30 =
        channels.handles2C08[
            static_cast<std::size_t>(channel)];
    state.currentChannel2C38 = channel;

    if (state.currentHandle2C30 == -1 ||
        channel == -1) {
        return;
    }

    if (channels.active2C28[
            static_cast<std::size_t>(channel)] == 0) {
        // Preserve the original oddity: currentChannel2C38 has already been
        // assigned above, so B824 immediately takes its same-channel return.
        selectWithValue(
            channel,
            branchValue,
            init,
            sample,
            channels,
            state,
            channelOps,
            ops);
        return;
    }

    const std::int32_t scaled =
        Code1AudioChannelsB598::scaledValue(
            init, channels, channelOps);

    ops.callED29(
        init.object2B90,
        state.currentHandle2C30,
        scaled,
        0);

    ops.callED01(
        init.object2B90,
        state.currentHandle2C30);

    if (branchValue != 0) {
        ops.callED29(
            init.object2B90,
            state.currentHandle2C30,
            0,
            0);

        // branchValue is again pushed to B598 but ignored there.
        const std::int32_t scaledAgain =
            Code1AudioChannelsB598::scaledValue(
                init, channels, channelOps);

        ops.callED29(
            init.object2B90,
            state.currentHandle2C30,
            scaledAgain,
            0);
    } else {
        ops.callED33(
            init.object2B90,
            state.currentHandle2C30,
            state.value2C40,
            0);
    }
}

void Code1AudioPlaybackB76C::loadAndSelect(
    std::int32_t resource,
    std::int32_t channel,
    const Code1AudioInitStateB0F8& init,
    const Code1SampleDataStateB060& sample,
    Code1AudioChannelsStateB598& channels,
    Code1AudioPlaybackStateB76C& state,
    const Code1AudioChannelsOpsB598& channelOps,
    const Code1AudioPlaybackOpsB76C& ops) {
    if (!init.initialized2B84 ||
        state.enabled2C44 == 0) {
        return;
    }

    stopCurrent(
        init, sample, channels, state,
        channelOps, ops);

    Code1AudioChannelsB598::clearAll(
        init, channels, channelOps);

    if (state.currentResource2C34 != resource) {
        std::int32_t item = resource;

        const std::uint16_t mode =
            configWord32(init);

        if (mode == 3 ||
            mode == 4) {
            item += channels.setting2B88;
        }

        const std::uintptr_t loaded =
            ops.ensureResource954C(
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

        ops.callD1E8(
            init.mainLinear2B9C,
            bytes + 4,
            length);

        state.currentResource2C34 = resource;
    }

    select(
        channel,
        init,
        sample,
        channels,
        state,
        channelOps,
        ops);
}

void Code1AudioPlaybackB76C::stopCurrent(
    const Code1AudioInitStateB0F8& init,
    const Code1SampleDataStateB060& sample,
    Code1AudioChannelsStateB598& channels,
    Code1AudioPlaybackStateB76C& state,
    const Code1AudioChannelsOpsB598& channelOps,
    const Code1AudioPlaybackOpsB76C& ops) {
    if (!init.initialized2B84 ||
        state.currentChannel2C38 == -1) {
        return;
    }

    const std::int32_t status =
        ops.callED0B(
            init.object2B90,
            state.currentHandle2C30);

    if (status != 0 &&
        status != 2) {
        ops.callECF7(
            init.object2B90,
            state.currentHandle2C30);
    }

    Code1AudioChannelsB598::start(
        0, init, sample, channels, channelOps);

    state.currentHandle2C30 =
        channels.handles2C08[0];
    state.currentChannel2C38 = 0;

    if (state.currentHandle2C30 != -1) {
        ops.callED29(
            init.object2B90,
            state.currentHandle2C30,
            0,
            0);

        ops.callED33(
            init.object2B90,
            state.currentHandle2C30,
            state.value2C40,
            0);

        ops.callECED(
            init.object2B90,
            state.currentHandle2C30);

        // Preserve original busy wait; no service/yield call occurs here.
        while (ops.callED0B(
                   init.object2B90,
                   state.currentHandle2C30) != 2) {
        }
    }

    state.currentChannel2C38 = -1;
    state.currentHandle2C30 = -1;
}

} // namespace ravenloft::game
