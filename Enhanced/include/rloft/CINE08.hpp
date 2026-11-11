#pragma once

#include "rloft/CINE07.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Original frames are [first,last). The first extracted frame receives two
// reconstructed visual packets when first>0, and one subtitle command when
// needed. Its original commands and timing remain in source order.
struct Cine08SegmentInfo final {
    std::size_t first{};
    std::size_t last{};
    std::size_t prefix_packets{};
    bool restored_subtitle{};
    std::uint64_t original_start_tick{};
    std::uint64_t duration_ticks{};
};

// Stateless visual context for a standalone subsection of any CODE.2 ANM.
// This reconstructs palette and chunky indexed frame at the boundary, then
// preserves the selected frame bytes verbatim (except first chunk count).
// It does NOT reconstruct prior sound/music playback or external text/fonts.
// The shared CINE00/CINE02/CINE07 codecs remain the only source decoders.
class Cine08File final {
public:
    explicit Cine08File(std::vector<std::uint8_t> source,
                        std::uint16_t alternate_image_wait = 6U);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] Cine08SegmentInfo segment_info(
        std::size_t first, std::size_t last) const;
    [[nodiscard]] std::vector<std::uint8_t> extract_visual_segment(
        std::size_t first, std::size_t last) const;
    [[nodiscard]] Cine02VisualState snapshot(std::size_t frame) const;
    [[nodiscard]] const Cine07FrameIndex& frame_index(std::size_t frame) const;
    [[nodiscard]] std::uint64_t total_ticks() const noexcept;

    // Verified literal-only 320x400 row-RLE, per original CODE.2 27BD8.
    // This encodes native color indices, NOT a 640-wide export.
    [[nodiscard]] static std::vector<std::uint8_t> encode_full_frame(
        const std::vector<std::uint8_t>& pixels);

    // Existing source-checked frame-range editing remains available.
    void apply(const Cine07Edits& edits);

private:
    Cine07File source_;
    [[nodiscard]] std::vector<std::uint8_t> active_subtitle(
        std::size_t before) const;
};

} // namespace rloft
