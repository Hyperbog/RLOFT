#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

struct Code1CharacterOptionState2198 {
    std::uint8_t field680C = 0;
    std::uint8_t field680D = 0;
    std::uint8_t field680E = 0;
};

class Code1CharacterOptions2198 {
public:
    // Direct reconstruction of CODE.1 0x2198..0x24D0.
    // Writes six 32-bit option flags and returns true when any flag is nonzero.
    // Modes 0/1 index original CODE.1 data tables at DS:06D8/06FC.
    static bool build(std::array<std::int32_t, 6>& out,
                      std::int32_t mode,
                      const Code1CharacterOptionState2198& state) noexcept;
};

} // namespace ravenloft::game
