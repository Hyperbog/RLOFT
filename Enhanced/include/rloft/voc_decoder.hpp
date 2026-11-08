#pragma once

#include <cstdint>
#include <vector>

namespace rloft {

struct PcmSound {
    std::uint32_t sample_rate{};
    std::uint16_t channels{};
    std::vector<std::int16_t> samples;
};

[[nodiscard]] PcmSound decode_creative_voice(
    const std::vector<std::uint8_t>& voc);

}  // namespace rloft
