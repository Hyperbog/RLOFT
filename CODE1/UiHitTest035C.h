#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

struct UiHitTestState035C {
    std::int32_t mode4D0 = 0;
    std::array<std::int32_t, 9> panelX{};
    std::array<std::int32_t, 9> panelY{};
    std::int32_t specialX8E4 = 0;
    std::int32_t specialY8A0 = 0;
};

class UiHitTest035C {
public:
    static std::int32_t hitTest(std::int32_t x,
                                std::int32_t y,
                                const UiHitTestState035C& state) noexcept;
};

} // namespace ravenloft::game
