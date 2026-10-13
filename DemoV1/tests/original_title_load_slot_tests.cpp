// Source-verified original CODE.2 07BB9..07C18 hit-test, not a
// native Load Game transaction or Windows screenshot acceptance.
#include "rloft/original_title_load_slot.hpp"

#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <limits>

using rloft::OriginalTitleLoadHitKind;
using rloft::original_title_load_hit;

static_assert(original_title_load_hit(21,20,0).slot == 1,
              "first selectable save is SAVE01");
static_assert(original_title_load_hit(354,91,0).slot == 10,
              "tenth selectable save is SAVE10");
static_assert(original_title_load_hit(378,91,0).slot == 10,
              "last originally selectable pixel");
static_assert(original_title_load_hit(379,91,0).kind ==
              OriginalTitleLoadHitKind::Outside,
              "x=379 must NOT be accepted");
static_assert(original_title_load_hit(21,20,2).kind ==
              OriginalTitleLoadHitKind::RightClickExit,
              "mouse-right exits BEFORE coordinate checks");
static_assert(original_title_load_hit(-1,-1,0x0102).kind ==
              OriginalTitleLoadHitKind::RightClickExit,
              "DOS uses only bit 1 of low 9E35 button byte");

int main() {
    // Each 37-pixel column contains 26 active X pixels, followed by
    // 11 inactive pixels, EXCEPT the tenth: original X<379 clips its
    // final active pixel (x=379) and the rest of that column.
    for (int column=0;column<10;++column) {
        const int left=21+37*column;
        for (int dx=0;dx<37;++dx) {
            const int x=left+dx;
            for (int y=20;y<92;++y) {
                const auto hit=original_title_load_hit(
                    static_cast<std::int16_t>(x),
                    static_cast<std::int16_t>(y),0);
                const bool active=x<379 && dx<=25;
                assert(hit.kind==(active
                    ? OriginalTitleLoadHitKind::SelectedSlot
                    : OriginalTitleLoadHitKind::Outside));
                assert(hit.slot==(active
                    ? static_cast<std::uint8_t>(column+1):0));
                const auto left_only=original_title_load_hit(
                    static_cast<std::int16_t>(x),
                    static_cast<std::int16_t>(y),1);
                assert(left_only.kind==hit.kind);
                assert(left_only.slot==hit.slot);
                for (const std::uint16_t buttons : {
                        std::uint16_t{2}, std::uint16_t{3},
                        std::uint16_t{0x102}, std::uint16_t{0xFFFE}}) {
                    const auto cancel=original_title_load_hit(
                        static_cast<std::int16_t>(x),
                        static_cast<std::int16_t>(y),buttons);
                    assert(cancel.kind==
                           OriginalTitleLoadHitKind::RightClickExit);
                    assert(cancel.slot==0);
                }
            }
        }
    }

    for (const int y : {19,92,-1,0,100,
                        static_cast<int>(std::numeric_limits<std::int16_t>::max()),
                        static_cast<int>(std::numeric_limits<std::int16_t>::min())}) {
        for (const int x : {21,50,350,378}) {
            assert(original_title_load_hit(
                static_cast<std::int16_t>(x),
                static_cast<std::int16_t>(y),0).kind ==
                OriginalTitleLoadHitKind::Outside);
        }
    }
    for (const int x : {20,379,-1,0,400,
                        static_cast<int>(std::numeric_limits<std::int16_t>::max()),
                        static_cast<int>(std::numeric_limits<std::int16_t>::min())}) {
        for (const int y : {20,91}) {
            assert(original_title_load_hit(
                static_cast<std::int16_t>(x),
                static_cast<std::int16_t>(y),0).kind ==
                OriginalTitleLoadHitKind::Outside);
        }
    }
    return 0;
}
