#pragma once

#include <cstdint>
#include <vector>

namespace rloft {

// CODE.1 loads exactly 0x3B bytes from SOUND.CFG into DGROUP 2AA0.
// B598 reads WORD [2AA0+32h]: value 3 applies a 90% correction to the
// configured relative sequence volume; every other value leaves it unchanged.
[[nodiscard]] int code1_music_relative_volume(
    const std::vector<std::uint8_t>& sound_cfg,
    int configured_relative_volume = 100);

}  // namespace rloft
