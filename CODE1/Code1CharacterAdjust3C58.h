#pragma once

#include "Code1TextDatabase.h"
#include "Code1CharacterSheet38A4.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1AdjustmentInput3C58 {
    std::int32_t selection = -1;     // return value from CODE.1 0x043C
    std::int32_t mouseButtons4D4 = 0;
    std::int16_t mouseX73E6 = 0;
};

struct Code1CharacterAdjustState3C58 {
    std::uint8_t field680C = 0;
    std::array<std::uint8_t, 3> fields680D{{0xFFu,0xFFu,0xFFu}};
    std::uint16_t word6810 = 0;
    std::uint8_t field6813 = 0;
    std::uint16_t word6816 = 0;
    std::array<std::uint16_t, 3> words6818{};
    std::array<std::uint8_t, 14> bytes682A{};
    std::int32_t global7118 = 0;
    std::int32_t global711C = 0;

    std::int32_t value924 = 0;
    std::int32_t value928 = 0;
    std::int32_t value92C = 0;
    std::int32_t value930 = 0;
    std::int32_t value934 = 0;
    std::int32_t value938 = 0;
    std::int32_t value940 = -1;

    std::uintptr_t handle954 = 0;
    std::uintptr_t handle958 = 0;

    std::int32_t selectedRow7124 = -1;
    std::uint8_t buffer7138 = 0;
    std::array<std::uint8_t, 9> control59F8{};
};

struct Code1CharacterAdjustOps3C58 {
    Code1CharacterSheetOps38A4 sheet;

    std::function<void(std::int32_t,std::int32_t)> call07E4;
    std::function<void()> call56DC;
    std::function<Code1AdjustmentInput3C58()> call043C;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::uint8_t,std::int32_t,std::int32_t,
                       std::int32_t,std::int32_t)> call55DC;
    std::function<std::int32_t()> call53B4;

    std::function<std::uintptr_t(std::int32_t,std::int32_t,
                                 std::int32_t,std::int32_t,std::int32_t)> call7868;
    std::function<void(std::int32_t,std::int32_t,
                       std::int32_t,std::int32_t,std::int32_t)> call9EB0;
    std::function<void(std::uintptr_t)> call7830;
    std::function<void(std::int32_t,std::int32_t,
                       std::int32_t,std::uintptr_t)> call67C8;
};

class Code1CharacterAdjust3C58 {
public:
    // Direct reconstruction of CODE.1 0x3C58..0x40FA.
    static void run(Code1CharacterAdjustState3C58& state,
                    const Code1TextDatabase& text,
                    const Code1CharacterAdjustOps3C58& ops);
};

} // namespace ravenloft::game
