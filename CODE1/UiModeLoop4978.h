#pragma once

#include "UiSequence45FC.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiModeLoopState4978 {
    std::int32_t mode4D0 = 0;
    std::int32_t value4CC = 0;
    std::uint8_t flag4C4 = 0;
    std::uint8_t flag4C5 = 0;
    std::uint8_t byte73EE = 0;
    UiSequenceState45FC sequence{};
};

struct UiModeLoopOps4978 {
    UiSequenceOps45FC sequence;

    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callC254;
    std::function<void()> call57DC;
    std::function<void(std::int32_t)> dispatchMode;
    std::function<std::int32_t()> call53B4;
};

class UiModeLoop4978 {
public:
    // Direct reconstruction of CODE.1 0x4978..0x4A09.
    // CODE.1 0x4A0A..0x4A0B is alignment.
    static std::int32_t run(UiModeLoopState4978& state,
                            const UiModeLoopOps4978& ops);
};

} // namespace ravenloft::game
