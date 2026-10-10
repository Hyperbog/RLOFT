#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_feedback_ribbon_epoch.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>

int main() {
    rloft::OriginalFeedbackRibbonEpoch epoch;
    std::array<std::uint8_t,128> widths{};
    widths.fill(1U);
    widths['A'] = 80U;
    int draws = 0;
    const auto next_random = [&]() -> std::uint16_t {
        ++draws;
        return static_cast<std::uint16_t>(draws % 3 == 0 ? 32767 : 0);
    };

    epoch.enter_modal_13118();
    assert(!epoch.present_vga_frame());
    assert(epoch.original_f790_calls() == 0U);
    // Source 112EC -> F790: ordinary, non-pending initial state.
    epoch.source_f790_call(false,widths,"AA",next_random);
    assert(!epoch.present_vga_frame());
    assert(epoch.original_f790_calls() == 1U);
    assert(draws == 0);

    // Source 116F5/11757 sets 6DAC and deadline, then calls F790 once.
    epoch.source_f790_call(true,widths,"AA",next_random);
    assert(epoch.original_f790_calls() == 2U);
    const auto* initial = epoch.present_vga_frame();
    assert(initial && initial->text_width == 160);
    assert(initial->text_column == 80);
    assert(initial->tiles.size() == 23U);
    assert(draws == 18); // interior 30CA2 calls, never border/seam
    const auto first_frame = initial->tiles[2].frame;

    // Thousands of purely native VGA presentations must not advance
    // the original shared RNG or regenerate the original F66C art.
    for (int i = 0; i < 1000; ++i) {
        const auto* frame = epoch.present_vga_frame();
        assert(frame != nullptr);
        assert(frame->tiles[2].frame == first_frame);
    }
    assert(draws == 18);
    assert(epoch.original_f790_calls() == 2U);

    // A SECOND actual F790 call while pending really does generate
    // new random tiles; do not deduplicate separate original events.
    epoch.source_f790_call(true,widths,"AA",next_random);
    assert(draws == 36);
    assert(epoch.original_f790_calls() == 3U);

    // Source 13184 clears pending/deadline before the F790 restore.
    epoch.feedback_expired_13184();
    assert(!epoch.present_vga_frame());
    epoch.source_f790_call(false,widths,"",next_random);
    assert(draws == 36);
    assert(epoch.original_f790_calls() == 4U);
    assert(!epoch.present_vga_frame());

    epoch.enter_modal_13118();
    assert(epoch.original_f790_calls() == 0U);
    assert(!epoch.present_vga_frame());

    // Unproved high-byte glyphs do not silently consume RNG.
    bool invalid = false;
    try {
        epoch.source_f790_call(true,widths,"\x80",next_random);
    } catch (const std::out_of_range&) { invalid = true; }
    assert(invalid);
    assert(draws == 36);
    assert(epoch.original_f790_calls() == 0U);
}
