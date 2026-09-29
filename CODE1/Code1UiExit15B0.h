#pragma once

#include "Code1UiEntry10A4.h"
#include "Code1UiTextComposite00EC.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1UiExitState15B0 {
    std::int32_t mode4D0 = 0;
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 9> current8F8{};
    std::array<std::int32_t, 9> resource954{};
    std::int32_t specialResource958 = 0;
    std::int32_t global713C = 0;
    std::uint8_t buffer7138 = 0;
};

struct Code1UiExitOps15B0 {
    Code1UiTextDrawOps00EC draw;

    std::function<void()> call56DC;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::uint8_t)> call53E4;
    std::function<void(std::uint8_t)> call5454;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::uintptr_t, std::int32_t)> call6824;
    std::function<Code1ResourceView10A4(std::int32_t, std::int32_t)> call954C;
    std::function<std::uintptr_t(std::size_t)> call71C8;
    std::function<void(std::uintptr_t)> call7240;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call66C0;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6744;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::uintptr_t)> call67C8;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call86C4;
    std::function<void(std::int32_t)> call04B0;
};

class Code1UiExit15B0 {
public:
    // Direct reconstruction of CODE.1 0x15B0..0x1A00.
    static void animate(std::int32_t selectedSlot,
                        std::int32_t targetX,
                        std::int32_t targetY,
                        std::int32_t characterPanelStage,
                        Code1UiExitState15B0& state,
                        const Code1UiExitOps15B0& ops);
};

} // namespace ravenloft::game
