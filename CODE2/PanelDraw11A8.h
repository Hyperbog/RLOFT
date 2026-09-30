#pragma once
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct PanelDrawState11A8 {
    std::uint8_t valueC050 = 0;
    const void* context3E = nullptr; // original DS:0x258DC + 0x3E
};

struct PanelDrawOps11A8 {
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call296C0;
    std::function<void(std::int32_t)> call291B0;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,
                       const void*,std::int32_t)> call2978C;
};

class PanelDraw11A8 {
public:
    // Direct reconstruction of CODE.2 0x11A8..0x1226.
    static void run(const PanelDrawState11A8& state,
                    const PanelDrawOps11A8& ops);
};

} // namespace ravenloft::game
