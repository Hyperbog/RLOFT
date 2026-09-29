#pragma once

#include "games/ravenloft/data/TextDatabase.h"
#include "games/ravenloft/game/Code1CharacterInit3884.h"
#include "games/ravenloft/game/Code1CharacterSheet38A4.h"
#include "games/ravenloft/game/Code1RuntimeRng618A.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

struct Code1CharacterFinalizeState40FC {
    Code1CharacterInitState3884 character{};

    std::int32_t mode4D0 = 0;
    std::int32_t selectedRow7124 = -1;

    std::int32_t value924 = 0;
    std::int32_t value928 = 0;
    std::int32_t value92C = 0;
    std::int32_t value930 = 0;
    std::int32_t value934 = 0;
    std::int32_t value938 = 0;

    std::uintptr_t handle954 = 0;
    std::uintptr_t handle958 = 0;

    std::uint8_t buffer7138 = 0;
    std::array<std::uint8_t, 9> control59F8{};

    std::string string4B8;
    std::string text7140;
};

struct Code1CharacterFinalizeOps40FC {
    Code1CharacterSheetOps38A4 sheet;

    std::function<std::uint32_t()> call7F50;
    std::function<void(std::uint8_t)> call6588;
    std::function<std::uintptr_t(std::int32_t, std::int32_t,
                                 std::int32_t, std::int32_t,
                                 std::int32_t)> call7868;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t,
                       std::int32_t)> call9EB0;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::uintptr_t)> call67C8;
    std::function<void(std::uintptr_t)> call7830;

    std::function<void(std::int32_t,
                       const std::array<std::int32_t, 9>&,
                       std::int32_t, std::int32_t,
                       std::int32_t)> call1464;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t,
                       std::int32_t)> call6C60;
    std::function<void(std::uint8_t)> call53E4;
    std::function<std::int32_t()> call043C;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call15B0;
    std::function<void()> call3138;

    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, const char*,
                       std::int32_t)> call7D80;
    std::function<void()> call5780;
    std::function<void(std::uint8_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t)> callA3E8;
    std::function<void()> call57DC;
    std::function<void()> call3C58;
    std::function<void(std::int32_t, std::int32_t)> call07E4;
};

class Code1CharacterFinalize40FC {
public:
    // Direct reconstruction of CODE.1 0x40FC..0x44E0.
    static void run(Code1CharacterFinalizeState40FC& state,
                    Code1RuntimeRng618A& rng,
                    const ravenloft::data::TextDatabase& text,
                    const Code1CharacterFinalizeOps40FC& ops);
};

} // namespace ravenloft::game
