#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_feedback_ribbon_epoch.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>

int main() {
    using rloft::OriginalRandom;
    rloft::OriginalFeedbackRibbonEpoch epoch;
    std::array<std::uint8_t,128> widths{};
    widths.fill(1U);
    widths['A'] = 80U;
    OriginalRandom shared(0x19876543U);
    const auto initial_rng_state = shared.state();
    int tick_observations = 0;
    const auto tick = [&] {
        ++tick_observations;
        return 0xDEADBEEFU;
    };

    epoch.enter_modal_13118();
    assert(!epoch.present_vga_frame());
    assert(epoch.original_f790_calls() == 0U);
    // The initial 112EC -> F790 with no held item does not draw or seed.
    epoch.source_f790_call(false,-1,widths,"",shared,tick);
    assert(!epoch.present_vga_frame());
    assert(epoch.original_f790_calls() == 1U);
    assert(shared.state() == initial_rng_state);
    assert(tick_observations == 0);

    // Pending 6DAC takes priority even when the original hand is empty.
    epoch.source_f790_call(true,-1,widths,"AA",shared,tick);
    const auto* pending = epoch.present_vga_frame();
    assert(pending && pending->text_width == 160);
    assert(pending->text_column == 80);
    assert(pending->tiles.size() == 23U);
    assert(epoch.original_f790_calls() == 2U);
    OriginalRandom expected(initial_rng_state);
    for (int i = 0; i < 18; ++i) (void)expected.next15();
    assert(shared.state() == expected.state());
    assert(tick_observations == 0);  // no reseed on pending branch
    const auto held_frame = pending->tiles[2].frame;
    for (int i = 0; i < 1000; ++i) {
        const auto* old = epoch.present_vga_frame();
        assert(old && old->tiles[2].frame == held_frame);
    }
    assert(shared.state() == expected.state());

    // A SECOND actual pending F790 call advances the gameplay RNG again.
    epoch.source_f790_call(true,74,widths,"AA",shared,tick);
    for (int i = 0; i < 18; ++i) (void)expected.next15();
    assert(shared.state() == expected.state());
    assert(tick_observations == 0);
    assert(epoch.original_f790_calls() == 3U);

    // Ordinary held-item F790 seeds 30CC4(item ID), draws 18 random
    // interior tiles, then samples 30700 and reseeds 30CC4(tick).
    epoch.source_f790_call(false,74,widths,"AA",shared,tick);
    assert(tick_observations == 1);
    assert(shared.state() == 0xDEADBEEFU);
    assert(epoch.original_f790_calls() == 4U);
    const auto* ordinary = epoch.present_vga_frame();
    assert(ordinary && ordinary->tiles.size() == 23U);
    OriginalRandom seed_from_item(74U);
    const auto expected_ordinary =
        rloft::OriginalFeedbackRibbonText::prepare(
            widths,"AA",[&] { return seed_from_item.next15(); });
    for (std::size_t i = 0; i < ordinary->tiles.size(); ++i) {
        assert(ordinary->tiles[i].column == expected_ordinary.tiles[i].column);
        assert(ordinary->tiles[i].frame == expected_ordinary.tiles[i].frame);
    }
    for (int i = 0; i < 1000; ++i)
        assert(epoch.present_vga_frame()->tiles[2].frame == ordinary->tiles[2].frame);
    assert(shared.state() == 0xDEADBEEFU);
    assert(tick_observations == 1);

    // Expiry invalidates pending art, but the F790 that immediately follows
    // MUST draw a new ordinary ribbon if the hand still contains an item.
    epoch.feedback_expired_13184();
    assert(!epoch.present_vga_frame());
    epoch.source_f790_call(false,74,widths,"AA",shared,tick);
    assert(epoch.present_vga_frame());
    assert(tick_observations == 2);
    assert(epoch.original_f790_calls() == 5U);

    epoch.enter_modal_13118();
    assert(!epoch.present_vga_frame());
    assert(epoch.original_f790_calls() == 0U);
    const auto before_invalid = shared.state();
    bool invalid = false;
    try {
        epoch.source_f790_call(false,74,widths,std::string(1,char(0x80)),shared,tick);
    } catch (const std::out_of_range&) { invalid = true; }
    assert(invalid);
    assert(shared.state() == before_invalid);
    assert(epoch.original_f790_calls() == 0U);
    assert(tick_observations == 2);
}
