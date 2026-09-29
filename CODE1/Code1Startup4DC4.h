#pragma once

#include "Code1PaletteTables4A0C.h"
#include "Code1ResourceLoad4C80.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1StartupState4DC4 {
    Code1PaletteState4A0C palette{};
    Code1ResourceLoadState4C80 resources{};

    std::uint8_t buffer7138 = 0;
    std::array<std::uint8_t, 0x920> records67F8{};
};

struct Code1StartupOps4DC4 {
    Code1PaletteOps4A0C paletteOps;
    Code1ResourceLoadOps4C80 resourceOps;

    std::function<void(std::int32_t, std::int32_t)> call95BC;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t)> call9724;
    std::function<void(std::int32_t)> call64DC;

    std::function<void(std::array<std::uint8_t, 0x300>&,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callC254;
    std::function<void(std::array<std::uint8_t, 0x300>&)> call6618;

    std::function<void()> call9AC4;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::uint8_t)> call65A4;
    std::function<void()> call52AC;
};

class Code1Startup4DC4 {
public:
    // Direct reconstruction of CODE.1 0x4DC4..0x4EED.
    // CODE.1 0x4EEE..0x4EEF is alignment.
    static void initialize(Code1StartupState4DC4& state,
                           const Code1StartupOps4DC4& ops);
};

struct Code1CharacterFinalizeOps4EF0 {
    std::function<void(std::int32_t)> call63A4;
    std::function<std::int32_t(std::int32_t)> call62D8;
    std::function<void(std::int32_t)> call62F0;
    std::function<std::int32_t(std::int32_t)> call62C0;
    std::function<void(const void*, std::int32_t,
                       std::int32_t)> call6320;
};

class Code1CharacterFinalize4EF0 {
public:
    // Direct reconstruction of CODE.1 0x4EF0..0x5013.
    static void run(std::array<std::uint8_t, 0x920>& records67F8,
                    const Code1CharacterFinalizeOps4EF0& ops);
};

} // namespace ravenloft::game
