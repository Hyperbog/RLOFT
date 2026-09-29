#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

class Code1CharacterClassId5D90 {
public:
    // Direct reconstruction of CODE.1 0x5D90..0x5F49.
    // CODE.1 0x5F4A..0x5F4B is the original two-byte MOV EAX,EAX NOP.
    static std::uint8_t classify(
        const std::array<std::uint8_t, 3>& fields680D) noexcept;
};

} // namespace ravenloft::game
