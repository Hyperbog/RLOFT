#pragma once

#include "rloft/startup_presenter.hpp"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>

namespace rloft {

// Original 112EC: 3104C(0x1400) -> 292BC saves the original VGA
// rectangle at y=0x62,x=0, height=0x10,width=0x140 into 7F68.
// Every F790 starts with 2932C, restoring that SAME saved rectangle.
// The source 2A114/2A178 transfer the four VGA planes sequentially;
// the indexed native equivalent preserves the full 5120 color indices.
// Source-only until complete F790 event and RNG ownership are connected.
class OriginalF790BackgroundPage final {
public:
    static constexpr int kRow = 0x62;
    static constexpr int kColumn = 0;
    static constexpr int kHeight = 0x10;
    static constexpr int kWidth = 0x140;
    static constexpr int kSavedPixels = kWidth * kHeight;

    void enter_modal_13118() noexcept { backing_.reset(); }

    // Original 112EC captures AFTER all four party columns and BEFORE
    // the first F790 call. Own a copy, not a mutable screen reference.
    void capture_112ec(const IndexedFramebuffer& screen) {
        auto next = screen.capture(kRow,kColumn,kHeight,kWidth);
        if (next.width != kWidth || next.height != kHeight ||
            next.pixels.size() != static_cast<std::size_t>(kSavedPixels))
            throw std::logic_error("invalid original F790 saved page extent");
        backing_ = std::move(next);
    }

    // Original F790 restores BEFORE checking 6DAC, even with empty hand.
    // Call once per source F790 event, never once per VGA presentation.
    void restore_2932c(IndexedFramebuffer& screen) const {
        if (!backing_)
            throw std::logic_error("original F790 backing page not captured");
        screen.restore(*backing_,kRow,kColumn);
    }

    // The original 13083 -> 310B0 frees its 7F68 allocation on exit.
    void leave_modal() noexcept { backing_.reset(); }

    [[nodiscard]] bool ready() const noexcept { return backing_.has_value(); }

private:
    std::optional<IndexedRegion> backing_;
};

} // namespace rloft
