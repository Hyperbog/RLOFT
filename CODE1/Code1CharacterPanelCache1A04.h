#pragma once

#include "Code1TextDatabase.h"
#include "Code1UiTextComposite00EC.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1CharacterPanelCacheState1A04 {
    std::uintptr_t handle95C = 0;
    std::uintptr_t handle960 = 0;
    std::uint8_t buffer7138 = 0;

    bool flag4C4 = false;
    bool flag4C5 = false;

    const char* record67F8 = "";
    std::int8_t field680A = 0;
    std::uint8_t field680B = 0;

    const char* record6A40 = "";
    std::int8_t field6A52 = 0;
    std::uint8_t field6A53 = 0;

    std::int32_t global713C = 0;
};

struct Code1CharacterPanelCacheOps1A04 {
    Code1UiTextDrawOps00EC draw;
    std::function<void(std::uintptr_t)> call7830;
    std::function<void(std::uint8_t)> call6588;
    std::function<std::uintptr_t(std::int32_t, std::int32_t,
                                 std::int32_t, std::int32_t,
                                 std::int32_t)> call7868;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::uintptr_t)> call67C8;
};

class Code1CharacterPanelCache1A04 {
public:
    // Direct reconstruction of CODE.1 0x1A04..0x1BD0.
    static void rebuild(Code1CharacterPanelCacheState1A04& state,
                        const Code1TextDatabase& text,
                        const Code1CharacterPanelCacheOps1A04& ops);
};

} // namespace ravenloft::game
