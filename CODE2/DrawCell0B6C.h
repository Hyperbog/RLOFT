#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct DrawCellState0B6C {
    std::span<const std::uint8_t> map1760;
    std::span<const std::uint8_t> flags198E0;
    std::span<const std::uint8_t> palette25822;
    std::int16_t width12756 = 0;

    std::uint8_t value764D = 0;
    std::uint8_t value7651 = 0;

    // Original DS:0x258DC + 0x34 object passed to 0x2975C.
    const void* drawContext = nullptr;
};

struct DrawCellOps0B6C {
    std::function<std::int32_t(std::int32_t, std::int32_t, std::int32_t)> call09B0;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       const void*, std::int32_t)> call2975C;
};

class DrawCell0B6C {
public:
    // Direct reconstruction of CODE.2 0x0B6C..0x0BF9.
    static void run(DrawCellState0B6C& state,
                    const DrawCellOps0B6C& ops,
                    std::int32_t x,
                    std::int32_t y,
                    std::int32_t arg3,
                    std::int32_t arg4);
};

} // namespace ravenloft::game
