#pragma once

#include "rloft/original_feedback_ribbon_layout.hpp"

#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace rloft {

// Independent original CODE.2 F66C -> 30288 / 30508 source model.
// No live Win32 modal changes, asset substitutions or RNG duplication.
// Original 30288 indexes the RES0 font-1 widths as unsigned bytes at
// metric+4, using a sign-extended input character. For the valid indexed
// 0..127 glyph domain, this is identical to the native Type2Font.widths.
struct OriginalFeedbackRibbonText final {
    struct Prepared final {
        int text_width{};
        int text_column{};
        std::vector<OriginalFeedbackRibbonLayout::Tile> tiles;
    };

    [[nodiscard]] static int measure(
        const std::array<std::uint8_t,128>& font1_widths,
        std::string_view text) {
        int width = 0;
        for (unsigned char c : text) {
            // Signed glyph indices 128..255 point before the original
            // width table. No proof exists for those invalid indices;
            // never invent an alternate glyph mapping.
            if (c >= font1_widths.size())
                throw std::out_of_range("CODE.2 30288 signed glyph index");
            if (font1_widths[c] >
                static_cast<unsigned>(std::numeric_limits<int>::max()-width))
                throw std::overflow_error("CODE.2 30288 text width overflow");
            width += font1_widths[c];
        }
        return width;
    }

    // CODE.2 30508 uses SAR 1, i.e. floor((span-width)/2), not C++
    // truncation toward zero for a negative odd difference.
    [[nodiscard]] static int centered_column(int text_width) noexcept {
        const int remainder =
            OriginalFeedbackRibbonLayout::kTextWidth - text_width;
        return remainder >= 0 ? remainder / 2 :
            -1 - (-(remainder + 1)) / 2;
    }

    // F66C computes width exactly once, then calls 2975C once for every
    // selected tile and 30508 for the final string. The caller-supplied
    // next_rand15 MUST reference the *shared* original 30CA2 RNG state.
    // Re-executing prepare advances that state, just like another F66C call.
    template<class NextRandom15>
    [[nodiscard]] static Prepared prepare(
        const std::array<std::uint8_t,128>& font1_widths,
        std::string_view text,
        NextRandom15&& next_rand15) {
        Prepared out;
        out.text_width = measure(font1_widths, text);
        out.text_column = centered_column(out.text_width);
        out.tiles = OriginalFeedbackRibbonLayout::plan(
            out.text_width, std::forward<NextRandom15>(next_rand15));
        return out;
    }
};

}  // namespace rloft
