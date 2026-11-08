#pragma once

#include "rloft/original_f790_caller_catalog.hpp"

#include <cstdint>
#include <optional>

namespace rloft {

// Immutable CODE.2 source sequence 130A0..13117, not a claim
// that any particular Win32 click uses this routine. It emits
// ONLY the 130DE selected-item/known=0 F790 call boundary.
class OriginalF790SelectedPoll final {
public:
    static constexpr std::uint32_t kSourceE8=0x130deU;
    struct SourceEvent final {
        std::uint32_t e8_offset{kSourceE8};
        std::int32_t item_id{-1};
        bool known{false};
    };

    void enter_loop_130a0() noexcept {previous_selected_=-1;}
    [[nodiscard]] std::int32_t previous_selected() const noexcept {
        return previous_selected_;
    }

    // The real source first invokes 1A1F4. If it signals loop exit,
    // branch 130B0 skips 9E33 selection processing altogether.
    [[nodiscard]] std::optional<SourceEvent> poll_130a9(
        bool source_input_finished,
        std::int16_t raw_selected_9e33,
        std::int32_t maximum_258e8) noexcept {
        if(source_input_finished)return std::nullopt;
        const auto raw=static_cast<std::int32_t>(raw_selected_9e33);
        // 130B9 compares RAW selection to ESI, initially -1;
        // 130BD..130CB clamps only the selected ID above max.
        // If raw stays > maximum, the next poll can redraw AGAIN
        // because ESI stores the CLAMPED ID at 130F7, not raw.
        if(raw==previous_selected_)return std::nullopt;
        const std::int32_t selected=raw>maximum_258e8
            ? maximum_258e8:raw;
        previous_selected_=selected;
        return SourceEvent{kSourceE8,selected,false};
    }
private:
    std::int32_t previous_selected_{-1};
};

} // namespace rloft
