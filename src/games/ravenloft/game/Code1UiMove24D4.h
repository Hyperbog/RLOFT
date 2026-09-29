#pragma once

#include "games/ravenloft/game/Code1UiEntry10A4.h"
#include "games/ravenloft/game/Code1UiTextComposite00EC.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1UiMoveState24D4 {
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 9> current8F8{};
    std::uint8_t buffer7138 = 0;
};

struct Code1UiMoveOps24D4 {
    Code1UiTextDrawOps00EC draw;
    std::function<void()> call56DC;
    std::function<std::uintptr_t(std::size_t)> call71C8;
    std::function<void(std::uintptr_t)> call7240;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call66C0;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6744;
    std::function<Code1ResourceView10A4(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::uintptr_t)> call67C8;
    std::function<void(std::uint8_t)> call53E4;
};

class Code1UiMove24D4 {
public:
    // Direct reconstruction of CODE.1 0x24D4..0x266B.
    static void animate(std::int32_t slot,
                        std::int32_t targetX,
                        std::int32_t targetY,
                        Code1UiMoveState24D4& state,
                        const Code1UiMoveOps24D4& ops);
};

} // namespace ravenloft::game
