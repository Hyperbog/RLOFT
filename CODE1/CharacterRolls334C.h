#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterRollOps334C {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t, std::uint8_t)> writeByte;

    // CODE.1 0x0010. Arguments are the four pushed DWORD values in source
    // order; the returned DWORD is the original EAX bit pattern.
    std::function<std::uint32_t(std::uint32_t, std::uint32_t,
                                std::uint32_t, std::uint32_t)> call0010;

    // CODE.1 0x84A0: destination, fill byte/value, count.
    std::function<void(std::uint32_t, std::uint32_t, std::uint32_t)> call84A0;
};

class CharacterRolls334C {
public:
    // Direct reconstruction of CODE.1 0x334C..0x3403. Character and table
    // accesses remain raw and 32-bit wrapping is preserved.
    static void initialize(const CharacterRollOps334C& ops);
};

} // namespace ravenloft::game
