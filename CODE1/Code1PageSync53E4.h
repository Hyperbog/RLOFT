#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1PageSyncState53E4 {
    std::int16_t word71DA = 0;
    std::int16_t word73EC = 0;
    std::uint8_t byte73EE = 0;
    std::int32_t value97C = 0;

    std::array<std::array<std::uint8_t, 0x400>, 2> buffers73EF{};
};

struct Code1PageSyncOps53E4 {
    std::function<void()> call9AE8;
    std::function<void()> call57DC;
    std::function<void()> call5780;
    std::function<void()> call8788;

    std::function<void(std::uint8_t)> call65A4;
    std::function<void(std::uint8_t)> call8740;
    std::function<void(std::uint8_t)> callA390;

    std::function<void(std::array<std::uint8_t, 0x400>&,
                       const std::array<std::uint8_t, 0x400>&,
                       std::int32_t)> callD1E8;

    std::function<void(std::uint8_t,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callA3E8;
};

class Code1PageFlip53E4 {
public:
    // Direct reconstruction of CODE.1 0x53E4..0x5450.
    // CODE.1 0x5451..0x5453 is alignment.
    static void run(std::uint8_t page,
                    Code1PageSyncState53E4& state,
                    const Code1PageSyncOps53E4& ops);
};

class Code1PageSync5454 {
public:
    // Direct reconstruction of CODE.1 0x5454..0x550A.
    // CODE.1 0x550B is alignment.
    static void run(std::uint8_t page,
                    Code1PageSyncState53E4& state,
                    const Code1PageSyncOps53E4& ops);
};

class Code1RectSync550C {
public:
    // Direct reconstruction of CODE.1 0x550C..0x55DA.
    // CODE.1 0x55DB is alignment.
    static void run(std::uint8_t page,
                    std::int32_t x,
                    std::int32_t y,
                    std::int32_t width,
                    std::int32_t height,
                    Code1PageSyncState53E4& state,
                    const Code1PageSyncOps53E4& ops);
};

} // namespace ravenloft::game
