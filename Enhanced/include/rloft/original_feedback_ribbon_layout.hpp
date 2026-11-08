#pragma once

#include <cstdint>
#include <vector>

namespace rloft {

// Original CODE.2 F66C (file 04A590..04A68A), including the separate
// 30CA2 original 15-bit RNG call. This is a source-only model; do not use a
// presentation-local RNG when attaching it to live gameplay. It must advance
// the one original game RNG, and only on interior random-tile branches.
struct OriginalFeedbackRibbonLayout final {
    static constexpr int kArchiveSlot = 2;
    static constexpr int kResourceId = 3;
    static constexpr int kTileRow = 0x62;
    static constexpr int kTextRow = 0x65;
    static constexpr int kTextColumn = 0;
    static constexpr int kTextWidth = 0x140;
    static constexpr int kFont = 1;

    struct Tile final {
        int column{};
        int frame{};
    };

    // text_width is the nonnegative pixel width returned by 30288(text,1).
    // next_rand15 must reproduce original CODE.2 30CA2's 0..0x7FFF
    // output and shared-state semantics (including its missing-pointer case),
    // not a fresh UI-only generator.
    // No random number is consumed by cap or lane-seam tiles.
    template<class NextRandom15>
    [[nodiscard]] static std::vector<Tile> plan(
        int text_width, NextRandom15&& next_rand15) {
        // F681..F6DA: original signed half-width and 4-pixel roundup,
        // first corrected for 8-pixel phase, then for 80-pixel actor seams.
        // The original 16-bit FF FCh mask is *zero extended* to 32 bits.
        const int rounded_half = ((text_width + 16) / 2 + 3) & 0xFFFC;
        int left = 0xA0 - rounded_half;
        if (left % 8 == 0) left -= 4;
        if ((left + 4) % 0x50 == 0) left -= 8;

        const int last_column = 0x138 - left;
        const int bound = 0x140 - left;
        std::vector<Tile> result;
        // F6E4..F748: 2975C(frame, row=62h, column, resource=3, slot=2).
        for (int column = left; column < bound; column += 8) {
            int frame;
            if (column == left) {
                frame = 0;
            } else if (column >= last_column) {
                frame = 5;
            } else if ((column + 4) % 0x50 == 0) {
                frame = 1;
            } else {
                // F71C..F72A: (3 * 30CA2()) >> 15, then + 2.
                // 30CA2 returns 15 bits, so frame is exactly 2, 3 or 4.
                const std::uint32_t value =
                    static_cast<std::uint32_t>(next_rand15());
                frame = 2 + static_cast<int>((value * 3U) >> 15U);
            }
            result.push_back({column, frame});
        }
        return result;
    }
};

}  // namespace rloft
