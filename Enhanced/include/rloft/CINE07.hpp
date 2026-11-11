#pragma once

#include "rloft/CINE06.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// An indexed, validated source-frame span. Tick intervals are half-open.
// Provenance records the most recent packet that touched the corresponding
// visual layer; subtitle contents remain the responsibility of AnmCinematic.
struct Cine07FrameIndex final {
    std::size_t frame{};
    std::size_t byte_begin{};
    std::size_t byte_end{};
    std::uint64_t start_tick{};
    std::uint16_t duration_ticks{};
    std::optional<std::size_t> last_palette_frame;
    std::optional<std::size_t> last_image_frame;
    std::optional<std::size_t> active_subtitle_frame;
};

struct Cine07Seek final {
    Cine07FrameIndex frame;
    std::uint16_t tick_within_frame{};
};

// Expected bytes contain complete serialized original frames, including
// their chunk-count headers. [first,last) can be empty for insertion.
// Replacements are complete frames containing original ANM chunk packets.
struct Cine07RangeEdit final {
    std::size_t first{};
    std::size_t last{};
    std::vector<std::uint8_t> expected_frame_bytes;
    std::vector<Cine06Frame> replacements;
};

struct Cine07Edits final {
    std::optional<std::size_t> expected_frame_count;
    std::optional<std::uint16_t> expected_wait;
    std::optional<std::uint16_t> replacement_wait;
    std::vector<Cine07RangeEdit> ranges;
};

// General-purpose index/range editor for CODE.2 ANM cinematics. All image
// and command validation remains in the shared Cine00..Cine06 codec. An
// explicit alternate wait enables the same index for CINE12 (2 vs 6 ticks).
class Cine07File final {
public:
    explicit Cine07File(std::vector<std::uint8_t> source,
                        std::uint16_t alternate_image_wait = 6U);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] const std::vector<Cine07FrameIndex>& timeline() const noexcept;
    [[nodiscard]] std::uint64_t total_ticks() const noexcept;
    [[nodiscard]] const Cine07FrameIndex& frame_index(std::size_t frame) const;
    [[nodiscard]] Cine07Seek seek_tick(std::uint64_t tick) const;
    [[nodiscard]] Cine06Frame frame(std::size_t index) const;
    [[nodiscard]] Cine02VisualState snapshot(std::size_t frame) const;
    [[nodiscard]] std::vector<Cine01Cue> cues() const;

    // Exact serialized source frame bytes, without the ANM's 4-byte header.
    // extract_range returns a valid ANM stream but does not synthesize the
    // prior palette, image, or subtitle state needed for independent playback.
    [[nodiscard]] std::vector<std::uint8_t> frame_bytes(
        std::size_t first, std::size_t last) const;
    [[nodiscard]] std::vector<std::uint8_t> extract_range(
        std::size_t first, std::size_t last) const;

    // Non-overlapping original-indexed ranges are applied together, in source
    // order. All expected bytes and count/wait preconditions are validated;
    // the complete resulting archive is reparsed before any live mutation.
    void apply(const Cine07Edits& edits);

private:
    Cine06File source_;
    std::uint16_t image_wait_{};
    std::vector<Cine07FrameIndex> timeline_;
    std::uint64_t total_ticks_{};
    void index_source();
};

} // namespace rloft
