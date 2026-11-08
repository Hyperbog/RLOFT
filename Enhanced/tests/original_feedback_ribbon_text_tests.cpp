#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_feedback_ribbon_text.hpp"
#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>

int main() {
    using Text = rloft::OriginalFeedbackRibbonText;
    using Ribbon = rloft::OriginalFeedbackRibbonLayout;
    std::array<std::uint8_t,128> widths{};
    widths.fill(1);
    widths['A'] = 3;
    widths['B'] = 2;
    assert(Text::measure(widths, "ABBA") == 10);
    assert(Text::measure(widths, "") == 0);
    assert(Text::centered_column(0) == 160);
    assert(Text::centered_column(320) == 0);
    assert(Text::centered_column(319) == 0);
    assert(Text::centered_column(321) == -1);
    assert(Text::centered_column(323) == -2);

    int calls = 0;
    auto result = Text::prepare(widths, "ABBA", [&] {
        ++calls;
        return static_cast<std::uint16_t>(32767);
    });
    assert(result.text_width == 10);
    assert(result.text_column == 155);
    const auto expected = Ribbon::plan(10, [] { return 32767; });
    assert(result.tiles.size() == expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        assert(result.tiles[i].column == expected[i].column);
        assert(result.tiles[i].frame == expected[i].frame);
    }
    assert(calls == 2);

    // An interior tile draws random frames 2..4 and consumes one shared
    // RNG output. Border/end/seam tiles consume NONE.
    widths['C'] = 80;
    calls = 0;
    const auto medium = Text::prepare(widths, "CC", [&] {
        ++calls;
        return static_cast<std::uint16_t>(0);
    });
    assert(medium.text_width == 160);
    assert(medium.text_column == 80);
    assert(medium.tiles.size() == 23);
    assert(calls == 18);

    // Original signed byte indexing was not a CP437 remapping:
    // unproved negative glyph indices are rejected, never fabricated.
    bool invalid = false;
    try {
        (void)Text::measure(widths, std::string(1, char(0x80)));
    } catch (const std::out_of_range&) {
        invalid = true;
    }
    assert(invalid);
}
