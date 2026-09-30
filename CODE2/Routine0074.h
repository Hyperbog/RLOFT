#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct RoutineState0074 {
    // Raw CODE.2 data beginning at DS:0x14D78.
    std::span<std::uint8_t> table14D78;
    std::int16_t count258A2 = 0;
};

struct RoutineOps0074 {
    std::function<void(std::int32_t)> call16524;
    std::function<void(std::int32_t)> call1C124;
    std::function<void()> call1F1C8;
};

class Routine0074 {
public:
    // Direct reconstruction of CODE.2 0x0074..0x00B7.
    static void run(RoutineState0074& state,
                    const RoutineOps0074& ops);
};

} // namespace ravenloft::game
