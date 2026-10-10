#pragma once

#include "rloft/original_feedback_ribbon_text.hpp"
#include "rloft/original_random.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

namespace rloft {

// Source-only CODE.2 F790 event-to-VGA cache. Unwired to the live host.
// F790 calls F66C for pending feedback OR a nonempty ordinary held item.
// The ordinary branch additionally reseeds shared 30CA2 before and after
// F66C (FD58..FD79). Merely repainting the cached result is side-effect-free.
class OriginalFeedbackRibbonEpoch final {
public:
    using Prepared = OriginalFeedbackRibbonText::Prepared;

    void enter_modal_13118() noexcept {
        cached_.reset();
        source_calls_ = 0U;
    }

    template<class ObserveGameTick30700>
    void source_f790_call(
        bool pending_6dac,
        std::int32_t held_id,
        const std::array<std::uint8_t,128>& font1_widths,
        std::string_view message,
        OriginalRandom& shared_game_rng,
        ObserveGameTick30700&& observe_tick_30700) {
        // Original F7B4 pending guard has priority over held ID,
        // and F7DB sends the ordinary empty-hand sentinel -1 to FD7C.
        if (!pending_6dac && held_id == -1) {
            cached_.reset();
            ++source_calls_;
            return;
        }

        // Native defensive input validation before mutating shared RNG.
        // Original game strings use its own valid font bytes; unsupported
        // high-byte strings must not corrupt the state during translation.
        (void)OriginalFeedbackRibbonText::measure(font1_widths, message);

        if (!pending_6dac) {
            // Original FD58 -> 30CC4 seeds from signed held-item ID.
            shared_game_rng.seed(static_cast<std::uint32_t>(held_id));
        }
        auto prepared = OriginalFeedbackRibbonText::prepare(
            font1_widths, message,
            [&shared_game_rng]() { return shared_game_rng.next15(); });
        if (!pending_6dac) {
            // Original FD6E -> 30700, FD74 -> 30CC4: observe IRQ
            // AFTER the ribbon's random tile draws, never before.
            shared_game_rng.seed(
                static_cast<std::uint32_t>(
                    std::forward<ObserveGameTick30700>(observe_tick_30700)()));
        }
        cached_ = std::move(prepared);
        ++source_calls_;
    }

    // The original 13184 expiry clears pending then calls F790 again.
    // This invalidates the prior feedback artwork; subsequent source F790
    // must render an ordinary held-item ribbon if the hand is nonempty.
    void feedback_expired_13184() noexcept { cached_.reset(); }

    [[nodiscard]] const Prepared* present_vga_frame() const noexcept {
        return cached_ ? &*cached_ : nullptr;
    }
    [[nodiscard]] std::size_t original_f790_calls() const noexcept {
        return source_calls_;
    }

private:
    std::optional<Prepared> cached_;
    std::size_t source_calls_{};
};

}  // namespace rloft
