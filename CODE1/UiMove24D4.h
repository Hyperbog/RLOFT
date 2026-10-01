#pragma once

#include "UiEntry10A4.h"
#include "UiTextComposite00EC.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiMoveState24D4 {
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 9> current8F8{};

    // DS:7138 is DWORD storage. 0x24D4 uses MOVZX BYTE for 0x6588, but uses
    // full DWORD loads for local-array indexing and 0x53E4. XOR changes only
    // the low byte at 0x263B.
    std::uint32_t buffer7138 = 0;
};

struct UiMoveOps24D4 {
    UiTextDrawOps00EC draw;
    std::function<void()> call56DC;
    std::function<std::uint32_t(std::uint32_t)> call71C8;
    std::function<void(std::uint32_t)> call7240;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::uint32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call66C0;
    std::function<void(std::uint32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6744;
    std::function<ResourceView10A4(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::uintptr_t)> call67C8;
    std::function<void(std::uint32_t)> call53E4;
};

class UiMove24D4 {
public:
    // Fidelity reconstruction of CODE.1 0x24D4..0x266B.
    static void animate(std::int32_t slot,
                        std::int32_t targetX,
                        std::int32_t targetY,
                        UiMoveState24D4& state,
                        const UiMoveOps24D4& ops);
};

} // namespace ravenloft::game
