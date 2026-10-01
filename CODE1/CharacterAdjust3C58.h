#pragma once

#include "CharacterNormalize3404.h"
#include "CharacterSheet38A4.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterAdjustOps3C58 {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint16_t(std::uint32_t)> readWord;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t,std::uint8_t)> writeByte;
    std::function<void(std::uint32_t,std::uint16_t)> writeWord;
    std::function<void(std::uint32_t,std::uint32_t)> writeDword;
    CharacterNormalizeOps3404 normalize;
    CharacterSheetOps38A4 sheet;
    std::function<void(std::uint32_t,std::uint32_t)> call07E4;
    std::function<void()> call56DC;
    std::function<std::uint32_t()> call043C;
    std::function<void(std::uint32_t)> call6588;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> call55DC;
    std::function<std::uint32_t()> call53B4;
    std::function<std::uint32_t(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> call7868;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> call9EB0;
    std::function<void(std::uint32_t)> call7830;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)> call67C8;
};

class CharacterAdjust3C58 {
public:
    // Direct reconstruction of CODE.1 0x3C58..0x40FA.
    static void run(const CharacterAdjustOps3C58& ops);
};

} // namespace ravenloft::game
