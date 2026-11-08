#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_feedback_ribbon_layout.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

int main() {
    using Ribbon = rloft::OriginalFeedbackRibbonLayout;
    static_assert(Ribbon::kArchiveSlot == 2 && Ribbon::kResourceId == 3);
    static_assert(Ribbon::kTileRow == 0x62 && Ribbon::kTextRow == 0x65);
    static_assert(Ribbon::kTextColumn == 0 && Ribbon::kTextWidth == 320);
    static_assert(Ribbon::kFont == 1);

    // Original signed, rounded, actor-boundary corrected positioning.
    int draws = 0;
    const auto short_tiles = Ribbon::plan(0, [&] { ++draws; return 0U; });
    assert(short_tiles.size() == 3U && draws == 0);
    assert(short_tiles[0].column == 148 && short_tiles[0].frame == 0);
    assert(short_tiles[1].column == 156 && short_tiles[1].frame == 1);
    assert(short_tiles[2].column == 164 && short_tiles[2].frame == 5);

    // F6AD/F6C3 original seam checks, tiles still span 80-pixel lanes.
    const auto medium = Ribbon::plan(160, [&] { ++draws; return 0U; });
    assert(medium.size() == 23U);
    assert(medium.front().column == 68 && medium.front().frame == 0);
    assert(medium.back().column == 244 && medium.back().frame == 5);
    assert(medium[1].column == 76 && medium[1].frame == 1);
    assert(medium[11].column == 156 && medium[11].frame == 1);
    assert(medium[21].column == 236 && medium[21].frame == 1);
    assert(draws == 18); // only non-cap, non-seam tiles advance RNG

    // Original frames are random 2,3,4 (not modulo, not 0..5 random).
    std::vector<std::uint32_t> randoms{0U,10922U,10923U,21845U,21846U,32767U};
    std::size_t index = 0;
    const auto sample = Ribbon::plan(80, [&] {
        const auto value = randoms[index % randoms.size()];
        ++index;
        return value;
    });
    std::vector<int> interior;
    for (const auto& tile : sample)
        if (tile.frame >= 2 && tile.frame <= 4)
            interior.push_back(tile.frame);
    assert(interior.size() == 10U);
    assert((std::vector<int>(interior.begin(),interior.begin()+6) ==
            std::vector<int>{2,2,3,3,4,4}));
    assert(index == interior.size());

    // Original may blit clipped tiles outside the 320-pixel viewport.
    const auto wide = Ribbon::plan(304, [&] { return 32767U; });
    assert(wide.size() == 43U);
    assert(wide.front().column == -12 && wide.front().frame == 0);
    assert(wide.back().column == 324 && wide.back().frame == 5);
    assert(wide[1].column == -4 && wide[1].frame == 1);
    assert(wide[41].column == 316 && wide[41].frame == 1);
}
