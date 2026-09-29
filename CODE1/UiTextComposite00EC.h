#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiTextDrawOps00EC {
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t, std::int32_t)> call6EB0;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6C60;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       const char*, std::int32_t)> call7D80;
};

class UiTextComposite00EC {
public:
    // Direct reconstruction of CODE.1 0x00EC..0x035B.
    static void draw(std::int32_t x,
                     std::int32_t y,
                     std::int32_t width,
                     std::int32_t textWidth,
                     const char* text,
                     std::int32_t mode,
                     std::int32_t surfaceOrResource,
                     const UiTextDrawOps00EC& ops);
};

} // namespace ravenloft::game
