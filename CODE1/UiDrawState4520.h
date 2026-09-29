#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiDrawStateOps4520 {
    std::function<std::uintptr_t(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::int32_t, std::uintptr_t)> callA5C8;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6C98;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6C60;
};

class UiDrawState4520 {
public:
    // Direct reconstruction of CODE.1 0x4520..0x45EE.
    //
    // CODE.1 0x45EF..0x45F8 is the 10-byte embedded sequence
    // "678456789:" used by the following routine.
    // CODE.1 0x45F9..0x45FB is alignment.
    static void draw(std::int32_t value,
                     std::int32_t delta,
                     const UiDrawStateOps4520& ops);
};

} // namespace ravenloft::game
