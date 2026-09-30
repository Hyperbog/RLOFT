#pragma once

#include <cstddef>
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

    // Original CODE.1 heap entry points used by 0x00EC.  Keeping allocation
    // and release explicit preserves call ordering and the original temporary
    // buffer lifetime instead of replacing it with a C++ container.
    std::function<std::uintptr_t(std::size_t)> call71C8;
    std::function<void(std::uintptr_t)> call7240;
};

class UiTextComposite00EC {
public:
    // Fidelity reconstruction of CODE.1 object-1 0x00EC..0x035B.
    // Argument mapping is the original stack contract:
    // +08 x, +0C y, +10 width, +14 textWidth, +18 text,
    // +1C mode, +20 surface/resource.
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
