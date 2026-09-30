#pragma once

#include <cstdint>
#include <span>

namespace ravenloft::game {

struct EntitySelectorState0010 {
    // Raw CODE.2 data beginning at DS:0x22230. The original walks records with
    // a 0x16-byte stride while touching fields through +0x1C, so keep this as
    // raw storage until the overlapping/adjacent layout is fully established.
    std::span<std::uint8_t> table22230;
    std::int16_t count258A6 = 0;
};

class EntitySelector0010 {
public:
    // Direct reconstruction of CODE.2 0x0010..0x0072.
    // Finds the first entry whose +0x16 word is 0x54 and whose signed +0x0E
    // and +0x0A words match the two arguments. On a match, bits 6..8 of the
    // word at +0x1C are replaced with selector value 1 (bit 6 set).
    static void run(EntitySelectorState0010& state,
                    std::int32_t first,
                    std::int32_t second) noexcept;
};

} // namespace ravenloft::game
