#pragma once

#include "games/ravenloft/data/TextDatabase.h"
#include "games/ravenloft/game/Code1UiTextComposite00EC.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1UiPageState064C {
    std::int32_t mode4D0 = 0;
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 9> value8F8{};
    std::array<std::int32_t, 9> resource954{};
    std::int32_t specialResource958 = 0;
    std::int32_t global713C = 0;
    std::int32_t textIndex940 = -1;
    std::int32_t textIndex948 = -1;
};

struct Code1UiPageOps064C {
    Code1UiTextDrawOps00EC draw;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call67C8;
};

class Code1UiPage064C {
public:
    // Direct reconstruction of CODE.1 0x064C..0x07E1.
    static void draw(const Code1UiPageState064C& state,
                     const ravenloft::data::TextDatabase& text,
                     const Code1UiPageOps064C& ops);
};

} // namespace ravenloft::game
