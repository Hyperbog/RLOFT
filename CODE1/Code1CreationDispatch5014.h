#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1CreationDispatchState5014 {
    std::int32_t handle4C8 = 0;
    std::uint8_t flag4C4 = 0;
    std::uint8_t flag4C5 = 0;
};

struct Code1CreationDispatchOps5014 {
    std::function<void(std::int32_t)> callBB80;
    std::function<void()> callBCB4;
    std::function<void(std::int32_t)> call62F0;
    std::function<void()> call538C;
    std::function<void()> call9778;
    std::function<void(std::int32_t)> call64DC;
    std::function<void()> call5F4C;
    std::function<void()> call4EF0;
    std::function<void(std::int32_t)> callCBF0;
};

class Code1CreationFinalize5014 {
public:
    // Direct reconstruction of CODE.1 0x5014..0x5089.
    // CODE.1 0x508A..0x508B is alignment.
    static void run(std::int32_t result,
                    Code1CreationDispatchState5014& state,
                    const Code1CreationDispatchOps5014& ops);
};

struct Code1CreationEntryOps508C {
    std::function<void(std::int32_t)> call61D0;
    std::function<void(std::int32_t)> callCBF0;
    std::function<void()> call4DC4;
    std::function<std::int32_t()> call4978;
    std::function<void(std::int32_t)> call5014;
};

class Code1CreationEntry508C {
public:
    // Direct reconstruction of CODE.1 0x508C..0x50BF.
    static void run(std::int32_t mode,
                    const Code1CreationEntryOps508C& ops);
};

} // namespace ravenloft::game
