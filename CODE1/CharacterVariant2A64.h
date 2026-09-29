#pragma once

#include "UiHitTest035C.h"
#include "UiSelection043C.h"
#include "UiTextComposite00EC.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterVariantRecord2A64 {
    bool active4C4 = false;
    std::int8_t field680A = 0;
    std::uint8_t field680B = 0;
};

struct CharacterVariantState2A64 {
    std::array<CharacterVariantRecord2A64, 2> characters{};
    std::int32_t selectedCharacter4CC = 0;
    std::int32_t mode4D0 = 0;
    std::int32_t global713C = 0;
    std::uint8_t buffer7138 = 0;

    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 9> previous8F4{};
    std::array<std::int32_t, 9> current8F8{};
    std::int32_t specialX8E4 = 0;
    std::int32_t specialY8A0 = 0;
};

struct CharacterVariantOps2A64 {
    UiTextDrawOps00EC draw;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::int32_t,
                       const std::array<std::int32_t, 9>&,
                       std::int32_t, std::int32_t, std::int32_t)> call1464;
    std::function<void(std::int32_t, std::int32_t)> call07E4;
    std::function<PointerSample043C()> pollPointer;
    std::function<void(std::uint8_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call55DC;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call15B0;
};

class CharacterVariant2A64 {
public:
    // Direct reconstruction of CODE.1 0x2A64..0x2DF8.
    static void run(CharacterVariantState2A64& state,
                    const CharacterVariantOps2A64& ops);
};

} // namespace ravenloft::game
