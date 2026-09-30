#pragma once

#include "UiTextComposite00EC.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiPageState064C {
    std::int32_t mode4D0 = 0;
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 9> value8F8{};
    std::array<std::int32_t, 9> resource954{};
    std::int32_t specialResource958 = 0;
    std::int32_t global713C = 0;
    std::int32_t textIndex940 = -1;
    std::int32_t textIndex948 = -1;
    char* textBuffer55F8 = nullptr;
};

struct UiPageOps064C {
    UiTextDrawOps00EC draw;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call67C8;
    // CODE.1 0x004C: destination buffer first, signed record index second.
    std::function<void(char*, std::int32_t)> call004C;
};

class UiPage064C {
public:
    // Fidelity-audited reconstruction of CODE.1 0x064C..0x07E3.
    static void draw(const UiPageState064C& state,
                     const UiPageOps064C& ops);
};

} // namespace ravenloft::game
