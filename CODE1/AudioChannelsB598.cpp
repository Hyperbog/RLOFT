#include "AudioChannelsB598.h"

#include <bit>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint16_t configWord32(
    const std::array<std::uint8_t, 0x100>& config) noexcept {
    return static_cast<std::uint16_t>(
        config[0x32]) |
        (static_cast<std::uint16_t>(
             config[0x33]) << 8u);
}

} // namespace

std::int32_t AudioChannelsB598::scaledValue(
    const AudioInitStateB0F8& init,
    AudioChannelsStateB598& state,
    const AudioChannelsOpsB598& ops) {
    ops.bindConfig8A18(
        init.config2AA0);

    if (configWord32(*init.config2AA0) == 3) {
        // Preserve exact 32-bit arithmetic:
        // (((value*4-value)*2)<<4 - (value*4-value)*2 + 50) / 100
        // = (90*value + 50) / 100, with x86 wrapping before signed IDIV.
        const std::uint32_t input =
            static_cast<std::uint32_t>(
                state.value2C3C);

        std::uint32_t eax =
            input * 4u;
        eax -= input;
        eax += eax;

        const std::uint32_t sixTimes =
            eax;

        eax <<= 4u;
        eax -= sixTimes;
        eax += 50u;

        const std::int32_t numerator =
            std::bit_cast<std::int32_t>(eax);

        return numerator / 100;
    }

    return state.value2C3C;
}

void AudioChannelsB598::start(
    std::int32_t channel,
    const AudioInitStateB0F8& init,
    const SampleDataStateB060& sample,
    AudioChannelsStateB598& state,
    const AudioChannelsOpsB598& ops) {
    if (!init.initialized2B84) {
        return;
    }

    const std::size_t index =
        static_cast<std::size_t>(channel);

    if (state.active2C28[index] != 0) {
        return;
    }

    state.handles2C08[index] =
        ops.callEC93(
            init.object2B90,
            init.mainLinear2B9C,
            channel,
            init.blockLinear2BA8[index],
            0);

    if (state.handles2C08[index] == -1) {
        return;
    }

    if (sample.loaded2B94 != 0) {
        for (;;) {
            const std::uint16_t code =
                ops.callECBB(
                    init.object2B90,
                    state.handles2C08[index]);

            if (code == 0xFFFFu) {
                break;
            }

            // Original converts AX into quotient/remainder by 0x100.
            const std::uint16_t high =
                static_cast<std::uint16_t>(
                    code / 0x100u);
            const std::uint16_t low =
                static_cast<std::uint16_t>(
                    code % 0x100u);

            const std::uintptr_t samplePointer =
                ops.findSampleB4D4(
                    high,
                    low);

            if (samplePointer == 0) {
                return;
            }

            ops.callECC5(
                init.object2B90,
                high,
                low,
                samplePointer);
        }
    }

    state.active2C28[index] = 1;
}

void AudioChannelsB598::stop(
    std::int32_t channel,
    const AudioInitStateB0F8& init,
    AudioChannelsStateB598& state,
    const AudioChannelsOpsB598& ops) {
    if (init.initialized2B84) {
        ops.callEC9D(
            init.object2B90,
            state.handles2C08[
                static_cast<std::size_t>(channel)]);
    }
}

void AudioChannelsB598::clearAll(
    const AudioInitStateB0F8& init,
    AudioChannelsStateB598& state,
    const AudioChannelsOpsB598& ops) {
    if (!init.initialized2B84) {
        return;
    }

    for (std::size_t index = 0;
         index < 8;
         ++index) {
        if (state.active2C28[index] == 0) {
            continue;
        }

        const std::int32_t status =
            ops.callED0B(
                init.object2B90,
                state.handles2C08[index]);

        if (status == 0 ||
            status == 2) {
            ops.callEC9D(
                init.object2B90,
                state.handles2C08[index]);
        }

        state.active2C28[index] = 0;
    }
}

void AudioChannelsB598::setSetting2B80(
    std::int32_t value,
    AudioChannelsStateB598& state) noexcept {
    state.setting2B80 = value;
}

void AudioChannelsB598::setSetting2B88(
    std::int32_t value,
    AudioChannelsStateB598& state) noexcept {
    state.setting2B88 = value;
}

} // namespace ravenloft::game
