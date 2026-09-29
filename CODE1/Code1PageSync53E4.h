#pragma once

#include "Code1InputState5230.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1PageSyncState53E4 {
    Code1InputState5230 input{};

    // Original global DS:097C.
    std::int32_t value97C = 0;

    // Original two 0x400-byte page-state buffers beginning at DS:73EF.
    std::array<std::array<std::uint8_t, 0x400>, 2> pageBuffers73EF{};
};

struct Code1PageSyncOps53E4 {
    std::function<void()> call9AE8;
    std::function<void(std::uint8_t)> call65A4;
    std::function<void(std::int32_t)> call8740;
    std::function<void(std::uint8_t)> callA390;
    std::function<void()> call8788;

    std::function<void(std::uint8_t,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callA3E8;

    std::function<void(std::uint8_t*,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call66C0;

    std::function<void(const std::uint8_t*,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6744;

    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, const std::uint8_t*)> call6D54;
};

class Code1PageSync53E4 {
public:
    // CODE.1 0x53E4..0x5450.
    static void flip(std::uint8_t page,
                     Code1PageSyncState53E4& state,
                     const Code1PageSyncOps53E4& ops);

    // CODE.1 0x5454..0x550A.
    static void flipAndClone(std::uint8_t page,
                             Code1PageSyncState53E4& state,
                             const Code1PageSyncOps53E4& ops);

    // CODE.1 0x550C..0x55DA.
    static void copyRectAndClone(std::uint8_t page,
                                 std::int32_t x,
                                 std::int32_t y,
                                 std::int32_t width,
                                 std::int32_t height,
                                 Code1PageSyncState53E4& state,
                                 const Code1PageSyncOps53E4& ops);

    // CODE.1 0x55DC..0x56DB.
    static void copyRect(std::uint8_t page,
                         std::int32_t x,
                         std::int32_t y,
                         std::int32_t width,
                         std::int32_t height,
                         Code1PageSyncState53E4& state,
                         const Code1PageSyncOps53E4& ops);

    // CODE.1 0x56DC..0x577C.
    static void syncAlternate(Code1PageSyncState53E4& state,
                              const Code1PageSyncOps53E4& ops);

    // CODE.1 0x5780..0x57DA.
    static void hideCursor(Code1PageSyncState53E4& state,
                           const Code1PageSyncOps53E4& ops);

    // CODE.1 0x57DC..0x5878.
    static void showCursor(Code1PageSyncState53E4& state,
                           const Code1PageSyncOps53E4& ops);
};

} // namespace ravenloft::game
