#pragma once

#include "rloft/CINE07.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace rloft {

// One ordered burst of frames sharing a CODE.2 playback timestamp. Frames
// [first_frame,last_frame) execute in source order, including zero-wait frames.
// If hold_ticks is nonzero, held_frame is the sole frame with an image-path
// wait in the burst; a final zero-only burst has no held_frame.
struct Cine10TickBatch final {
    std::size_t first_frame{};
    std::size_t last_frame{};
    std::uint64_t start_tick{};
    std::uint16_t hold_ticks{};
    std::optional<std::size_t> held_frame;
    std::optional<std::size_t> last_palette_frame;
    std::optional<std::size_t> last_image_frame;
    std::optional<std::size_t> active_subtitle_frame;
};

struct Cine10TimedEvent final {
    Cine01Cue cue;
    std::uint64_t start_tick{};
    std::size_t batch_index{};
    bool zero_wait_frame{};
    bool subtitle_clear{};
};

// A source-preserving, deterministic dispatch schedule for all CODE.2 ANMs.
// Particularly important when default_wait == 0 (CINE10 and CINE11):
// multiple palette, subtitle, and audio packets may execute at the same tick.
// This is a tick/frame scheduler, not a sampled-audio or subtitle renderer.
class Cine10File final {
public:
    explicit Cine10File(std::vector<std::uint8_t> source,
                        std::uint16_t alternate_image_wait = 6U);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] std::uint64_t total_ticks() const noexcept;
    [[nodiscard]] std::size_t zero_wait_frames() const noexcept;
    [[nodiscard]] const std::vector<Cine10TickBatch>& batches() const noexcept;
    [[nodiscard]] const std::vector<Cine10TimedEvent>& events() const noexcept;
    [[nodiscard]] const Cine10TickBatch& batch_for_frame(std::size_t frame) const;
    [[nodiscard]] const Cine10TickBatch& batch_for_tick(std::uint64_t tick) const;

    // Unlike batch_for_tick, event lookup accepts tick == total_ticks so that
    // terminal zero-wait commands are not silently discarded.
    [[nodiscard]] std::vector<Cine10TimedEvent> events_at_tick(
        std::uint64_t tick) const;
    [[nodiscard]] std::string event_csv() const;

    // Existing source-checked frame-range edits; rebuild the complete schedule
    // before publishing any change. An invalid edit leaves original bytes and
    // every cached schedule entry unchanged.
    void apply(const Cine07Edits& edits);

private:
    Cine07File source_;
    std::uint16_t image_wait_{};
    std::size_t zero_wait_frames_{};
    std::vector<Cine10TickBatch> batches_;
    std::vector<std::size_t> frame_to_batch_;
    std::vector<Cine10TimedEvent> events_;
    void index();
};

} // namespace rloft
