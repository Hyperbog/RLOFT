#include "CharacterSheet38A4.h"

#include "RuntimeDecimal88D8.h"
#include "RuntimeStrcatB01A.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

namespace ravenloft::game {
namespace {

// Exact signed-byte table at CODE.1 data DS:083F. The renderer indexes this
// with record byte 0x34 (DS:682C).
constexpr std::array<std::int8_t, 20> kModifier083F{{
    6,5,5,4,3,2,1,0,0,0,0,0,0,0,0,-1,-2,-3,-4,-4
}};

std::string formatTwo(std::uint8_t value) {
    char buffer[16]{};
    std::snprintf(buffer, sizeof(buffer), "%02d", static_cast<int>(value));
    return buffer;
}

std::string formatExceptional(std::uint8_t value, std::uint8_t percentile) {
    char buffer[24]{};
    const int remainder = static_cast<int>(percentile) % 100;
    std::snprintf(buffer, sizeof(buffer), "%02d/%02d",
                  static_cast<int>(value), remainder);
    return buffer;
}

void setHighlight(CharacterSheetState38A4& state,
                  const CharacterSheetOps38A4& ops,
                  bool enabled) {
    if (enabled) {
        state.control59F8[8] = 0x5A;
        ops.call7DF4(state.control59F8.data());
    } else {
        state.control59F8[8] = 0x08;
        ops.call7DF4(nullptr);
    }
}

} // namespace

void CharacterSheet38A4::draw(
    std::int32_t x,
    std::int32_t y,
    CharacterSheetState38A4& state,
    const TextDatabase& text,
    const CharacterSheetOps38A4& ops) {
    // 0x38A9..0x38BF.
    ops.call6C60(0, x, y, 0x14, 7);
    x += 4;

    // 0x38C3..0x39AF: six alternating ability-score fields.
    for (std::int32_t row = 0; row < 6; ++row) {
        const bool highlighted = row == state.selectedRow7124;
        if (highlighted) setHighlight(state, ops, true);

        const auto& label = text.at(static_cast<std::size_t>(row + 0x2F));
        ops.call7E44(x, y + 1, label, 0);

        const auto value = state.bytes682A[static_cast<std::size_t>(row) * 2u];
        std::string rendered;
        if (row == 0 && state.bytes682A[12] != 0u) {
            rendered = formatExceptional(value, state.bytes682A[12]);
        } else {
            rendered = formatTwo(value);
        }
        ops.call7E04(x, y + 0x2F, rendered, 0);

        if (highlighted) setHighlight(state, ops, false);
        x += 0x0A;
    }

    // 0x39B5..0x3A0F: record 0x35 and signed WORD 6810.
    x += 0x0A;
    ops.call7E44(x, y + 1, text.at(0x35), 0);
    ops.call7E04(
        x, y + 0x2F,
        RuntimeDecimal88D8::convert(
            static_cast<std::int16_t>(state.word6810)),
        0);

    // 0x3A12..0x3A7D: record 0x36 and 6813 + DS:083F[682C].
    x += 0x0A;
    ops.call7E44(x, y + 1, text.at(0x36), 0);
    const auto modifierIndex =
        static_cast<std::size_t>(state.bytes682A[2]);
    const std::int32_t derived =
        static_cast<std::int8_t>(state.field6813) +
        static_cast<std::int32_t>(kModifier083F[modifierIndex]);
    ops.call7E04(
        x, y + 0x2F,
        RuntimeDecimal88D8::convert(derived),
        0);

    // 0x3A80..0x3BB6: label 0x37 for one class, 0x38 for multiple classes,
    // followed by slash-separated signed WORD levels.
    x += 0x0A;
    const bool hasSecond =
        static_cast<std::int8_t>(state.fields680D[1]) != -1;
    const bool hasThird =
        static_cast<std::int8_t>(state.fields680D[2]) != -1;
    ops.call7E44(x, y + 1, text.at(hasSecond ? 0x38 : 0x37), 0);

    std::string levels =
        RuntimeDecimal88D8::convert(
            static_cast<std::int16_t>(state.words6818[0]));
    if (hasSecond) {
        RuntimeStrcatB01A::append(levels, "/");
        RuntimeStrcatB01A::append(
            levels,
            RuntimeDecimal88D8::convert(
                static_cast<std::int16_t>(state.words6818[1])));
    }
    if (hasThird) {
        RuntimeStrcatB01A::append(levels, "/");
        RuntimeStrcatB01A::append(
            levels,
            RuntimeDecimal88D8::convert(
                static_cast<std::int16_t>(state.words6818[2])));
    }
    ops.call7E04(x, y + 0x2F, levels, 0);

    // 0x3BB9..0x3C4E: record 0x39 and signed WORD 6816.
    x += 0x0A;
    const bool finalHighlighted = state.selectedRow7124 == 0x0A;
    if (finalHighlighted) setHighlight(state, ops, true);

    ops.call7E44(x, y + 1, text.at(0x39), 0);
    ops.call7E04(
        x, y + 0x2F,
        RuntimeDecimal88D8::convert(
            static_cast<std::int16_t>(state.word6816)),
        0);

    if (finalHighlighted) setHighlight(state, ops, false);
}

} // namespace ravenloft::game
