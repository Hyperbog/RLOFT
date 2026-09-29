#pragma once

#include "Code1UiDrawState4520.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1UiSequenceState45FC {
    std::uint8_t buffer7138 = 0;
    std::int32_t value712C = 0;
};

struct Code1UiSequenceOps45FC {
    Code1UiDrawStateOps4520 drawState;

    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call86C4;
    std::function<std::uintptr_t(std::int32_t, std::int32_t,
                                 std::int32_t, std::int32_t,
                                 std::int32_t)> call7868;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6C60;
    std::function<void(std::uint8_t)> callA390;
    std::function<void(std::uint8_t)> call65A4;
    std::function<std::uintptr_t(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::uintptr_t)> call67C8;
    std::function<void()> call87C4;
    std::function<void(std::int32_t, std::int32_t)> call87D8;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t)> callBC1C;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t,
                       std::int32_t)> callBFE0;
    std::function<void(std::int32_t)> callC4E0;
    std::function<void(std::int32_t, std::int32_t)> call91D8;
    std::function<void(std::uintptr_t)> call7830;
};

class Code1UiSequence45FC {
public:
    // Direct reconstruction of CODE.1 0x45FC..0x4976.
    //
    // CODE.1 0x45EF..0x45F8 is the embedded 10-byte resource sequence used
    // below; 0x45F9..0x45FB is alignment and is not executable code.
    static void run(Code1UiSequenceState45FC& state,
                    const Code1UiSequenceOps45FC& ops);
};

} // namespace ravenloft::game
