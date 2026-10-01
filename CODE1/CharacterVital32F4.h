#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterVitalOps32F4 {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint16_t(std::uint32_t)> readWord;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t, std::uint16_t)> writeWord;

    // CODE.1 0x0010. Arguments are the four pushed DWORD values in source
    // order; return value is the original EAX bit pattern.
    std::function<std::uint32_t(std::uint32_t, std::uint32_t,
                                std::uint32_t, std::uint32_t)> call0010;
};

class CharacterVital32F4 {
public:
    // Direct reconstruction of CODE.1 0x32F4..0x3349. All DS accesses remain
    // raw so out-of-range table indices and 32-bit address wrapping are not
    // sanitized by host containers.
    static void initialize(const CharacterVitalOps32F4& ops);
};

} // namespace ravenloft::game
