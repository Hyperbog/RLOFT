#include "rloft/sound_config.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

int code1_music_relative_volume(
    const std::vector<std::uint8_t>& sound_cfg,
    int configured_relative_volume) {
    if (sound_cfg.size() != 0x3BU) {
        throw std::runtime_error(
            "SOUND.CFG is not the CODE.1 59-byte audio configuration record");
    }

    const std::uint16_t kind =
        static_cast<std::uint16_t>(sound_cfg[0x32U]) |
        (static_cast<std::uint16_t>(sound_cfg[0x33U]) << 8U);

    // function_B598 uses this correction only for kind 3:
    // ((volume * 6) * 16 - volume * 6 + 50) / 100
    // == (90 * volume + 50) / 100.
    if (kind == 3U) {
        return (configured_relative_volume * 90 + 50) / 100;
    }
    return configured_relative_volume;
}

}  // namespace rloft
