#pragma once

#include "Code1UiTextComposite00EC.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1CharacterPanelState04B0 {
    const char* recordBase67F8 = nullptr;
    std::int8_t field680A = 0;
    std::uint8_t field680B = 0;
    std::uint8_t field680C = 0;
    std::array<std::int8_t, 3> fields680D{};
    std::uint8_t field6812 = 0;
    bool flag4C4 = false;
    std::int32_t global713C = 0;
};

struct Code1CharacterPanelOps04B0 {
    Code1UiTextDrawOps00EC draw;
    std::function<void(std::int32_t, std::int32_t)> call38A4;
};

class Code1CharacterPanel04B0 {
public:
    // Direct reconstruction of CODE.1 0x04B0..0x0648.
    static void drawThroughStage(std::int32_t stage,
                                 const Code1CharacterPanelState04B0& state,
                                 const Code1CharacterPanelOps04B0& ops);
};

} // namespace ravenloft::game
