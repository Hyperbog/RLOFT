#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CursorSyncState55DC {
    std::int16_t word71DA = 0;
    std::int16_t word73EC = 0;
    std::int16_t mouseX73E6 = 0;
    std::int16_t mouseY73E8 = 0;
    std::int16_t word73EA = 0;

    std::int16_t value71D8 = 0;
    std::int16_t value71D6 = 0;
    std::int16_t value71D4 = 0;

    std::uint8_t byte73EE = 0;
    std::uint8_t byte23E6 = 0;
    std::int32_t value97C = 0;

    std::array<std::array<std::uint8_t, 0x400>, 2> buffers73EF{};
    std::array<std::uint8_t, 0x20A> cursor71DC{};
};

struct CursorSyncOps55DC {
    std::function<void(std::uint8_t,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callA3E8;

    std::function<void(std::array<std::uint8_t, 0x400>&,
                       const std::array<std::uint8_t, 0x400>&,
                       std::int32_t)> callD1E8;

    std::function<void()> call5780;
    std::function<void()> call57DC;
    std::function<void()> call8788;

    std::function<void(std::uint8_t)> call8740;
    std::function<void(std::uint8_t)> callA390;

    std::function<void(const std::array<std::uint8_t, 0x400>&,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6744;

    std::function<void(std::array<std::uint8_t, 0x400>&,
                       std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call66C0;

    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t,
                       const std::array<std::uint8_t, 0x20A>&)> call6D54;
};

class ConditionalRectSync55DC {
public:
    // Direct reconstruction of CODE.1 0x55DC..0x56DB.
    static void run(std::uint8_t page,
                    std::int32_t x,
                    std::int32_t y,
                    std::int32_t width,
                    std::int32_t height,
                    CursorSyncState55DC& state,
                    const CursorSyncOps55DC& ops);
};

class CursorPageSync56DC {
public:
    // Direct reconstruction of CODE.1 0x56DC..0x577C.
    // CODE.1 0x577D..0x577F is alignment.
    static void run(CursorSyncState55DC& state,
                    const CursorSyncOps55DC& ops);
};

class CursorHide5780 {
public:
    // Direct reconstruction of CODE.1 0x5780..0x57DA.
    // CODE.1 0x57DB is alignment.
    static void run(CursorSyncState55DC& state,
                    const CursorSyncOps55DC& ops);
};

class CursorShow57DC {
public:
    // Direct reconstruction of CODE.1 0x57DC..0x5878.
    // CODE.1 0x5879..0x587F is non-executable zero/alignment data.
    static void run(CursorSyncState55DC& state,
                    const CursorSyncOps55DC& ops);
};

} // namespace ravenloft::game
