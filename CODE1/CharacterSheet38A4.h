#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterSheetOps38A4 {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint16_t(std::uint32_t)> readWord;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t,std::uint8_t)> writeByte;
    std::function<void(std::uint32_t,std::uint32_t)> call004C;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> call6C60;
    std::function<void(std::uint32_t)> call7DF4;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> call7E44;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> call7E04;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t)> callAFEE3;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> callAFEE4;
    std::function<std::uint32_t(std::uint32_t)> call88D8;
    std::function<void(std::uint32_t,std::uint32_t)> call72A0;
    std::function<void(std::uint32_t,std::uint32_t)> callB01A;
};

class CharacterSheet38A4 {
public:
    // Direct reconstruction of CODE.1 0x38A4..0x3C54.
    static void draw(std::uint32_t x, std::uint32_t y,
                     const CharacterSheetOps38A4& ops);
};

} // namespace ravenloft::game
