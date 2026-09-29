#pragma once

#include "TextDatabase.h"
#include "UiEntry10A4.h"
#include "UiTransition07E4.h"

#include <array>
#include <cstdint>
#include <string>

namespace ravenloft::game {

struct UiSetupState1464 {
    std::int32_t mode4D0 = 0;
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 11> previous8F4Stride8{};
    std::array<std::int32_t, 11> current8F8Stride8{};
    std::array<std::int32_t, 9> resource954{};
    std::int32_t specialResource958 = 0;
    std::array<std::uint8_t, 12> animation964{};
    std::uint8_t flag970 = 0;
    std::uint8_t buffer7138 = 0;
    std::uint8_t byte63F8 = 0;
    std::int32_t global713C = 0;
    std::string string4B8;
    std::string text7140;
};

struct UiSetupOps1464 {
    UiEntryOps10A4 entry;
    UiTransitionOps07E4 transition;
};

class UiSetup1464 {
public:
    // Direct reconstruction of CODE.1 0x1464..0x15AD.
    static void run(std::int32_t characterPanelStage,
                    const std::array<std::int32_t, 9>& slotValues,
                    std::int32_t textIndex940,
                    std::int32_t textIndex948,
                    std::int32_t titleSelector,
                    UiSetupState1464& state,
                    const TextDatabase& text,
                    const UiSetupOps1464& ops);
};

} // namespace ravenloft::game
