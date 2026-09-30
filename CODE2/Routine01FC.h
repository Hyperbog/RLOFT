#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct RoutineState01FC {
    std::span<std::uint8_t> characters20000;
    std::span<std::uint8_t> data1994C;
    std::int16_t value25619 = 0;
    std::int32_t valueBF44 = 0;
};

struct RoutineOps01FC {
    std::function<std::int32_t(std::int32_t)> call1C1DC;
    std::function<std::int32_t(std::int32_t)> call14A60;
    std::function<void(std::int32_t)> call1C124;
};

class Routine01FC {
public:
    // Direct reconstruction of CODE.2 0x01FC..0x0270.
    static std::int32_t run(RoutineState01FC& state,
                            const RoutineOps01FC& ops);
};

} // namespace ravenloft::game
