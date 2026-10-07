#include "rloft/sound_config.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

int main() {
    std::vector<std::uint8_t> cfg(0x3B, 0);

    // Supplied/certified sample: WORD +32h == 4 -> unchanged.
    cfg[0x32] = 4;
    cfg[0x33] = 0;
    assert(rloft::code1_music_relative_volume(cfg, 100) == 100);
    assert(rloft::code1_music_relative_volume(cfg, 73) == 73);

    // B598 special backend kind 3 -> rounded 90% correction.
    cfg[0x32] = 3;
    assert(rloft::code1_music_relative_volume(cfg, 100) == 90);
    assert(rloft::code1_music_relative_volume(cfg, 73) == 66);

    bool threw = false;
    try {
        (void)rloft::code1_music_relative_volume(
            std::vector<std::uint8_t>(0x3A, 0));
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);

    std::cout << "sound_config_tests: PASS\n";
}
