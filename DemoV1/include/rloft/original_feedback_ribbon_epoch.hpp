#pragma once

#include "rloft/original_feedback_ribbon_text.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

namespace rloft {

// Host-presentation adapter for original CODE.2 13118 -> 112EC -> F790
// and its event-driven F790 callsites. This cache is source-only and is
// NOT yet used by the native game. The original F66C advances shared 30CA2
// only when F790 explicitly invokes it with pending byte 6DAC != 0.
// A host's repeated VGA presents must never re-run that RNG-dependent draw.
class OriginalFeedbackRibbonEpoch final {
public:
    using Prepared = OriginalFeedbackRibbonText::Prepared;

    void enter_modal_13118() noexcept {
        // Original 13118 clears the 6DA8 deadline and 6DAC pending flag,
        // then invokes 112EC exactly once before its modal input loop.
        cached_.reset();
        source_calls_ = 0U;
    }

    template<class NextRandom15>
    void source_f790_call(
        bool pending_6dac,
        const std::array<std::uint8_t,128>& font1_widths,
        std::string_view message,
        NextRandom15&& next_rand15) {
        // F790's ordinary-item path does not invoke F66C.
        // Clear only this pending-feedback art; ordinary item rendering
        // remains the responsibility of a separate original source path.
        if (!pending_6dac) {
            cached_.reset();
            ++source_calls_;
            return;
        }
        // A *real* F790 event consumes one RNG value per interior tile.
        // Compute before replacing the previous cache to preserve it if
        // the host adapter's font or resource access throws.
        auto next = OriginalFeedbackRibbonText::prepare(
            font1_widths,message,std::forward<NextRandom15>(next_rand15));
        cached_ = std::move(next);
        ++source_calls_;
    }

    // Original 13184 calls F790 after clearing 6DA8/6DAC, on strict
    // unsigned tick > deadline. The caller must establish that predicate.
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
