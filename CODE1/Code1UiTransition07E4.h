#pragma once

#include "Code1TextDatabase.h"
#include "Code1UiTextComposite00EC.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1UiTransitionState07E4 {
    std::int32_t mode4D0 = 0;
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 9> previous8F4{};
    std::array<std::int32_t, 9> current8F8{};
    std::array<std::int32_t, 9> resource954{};
    std::int32_t specialResource958 = 0;
    std::int32_t global713C = 0;
    std::int32_t textIndex940 = -1;
    std::int32_t textIndex948 = -1;
    std::uint8_t buffer7138 = 0;
    std::uint8_t byte63F8 = 0;
};

struct Code1UiTransitionOps07E4 {
    Code1UiTextDrawOps00EC draw;

    std::function<void(std::int32_t, std::int32_t)> call8E3C;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call86C4;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call67C8;
    std::function<std::int32_t(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call9C44;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callA05C;
    std::function<void(std::uint8_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call55DC;
    std::function<void(std::uint8_t)> call53E4;
    std::function<void()> call064C;
};

class Code1UiTransition07E4 {
public:
    // Direct reconstruction of CODE.1 0x07E4..0x10A1.
    // startIndex/endIndex are the original half-open slot range arguments.
    static void animate(std::int32_t startIndex,
                        std::int32_t endIndex,
                        Code1UiTransitionState07E4& state,
                        const Code1TextDatabase& text,
                        const Code1UiTransitionOps07E4& ops);
};

} // namespace ravenloft::game
