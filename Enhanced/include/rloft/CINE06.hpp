#pragma once

#include "rloft/CINE02.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Raw ANM frame components. Packet payloads are the original CODE.2 byte
// streams; no second RLE/delta/planar decoder is introduced.
struct Cine06Packet final {
    std::uint16_t id{};
    std::vector<std::uint8_t> payload;
};

struct Cine06Frame final {
    std::vector<Cine06Packet> packets;
};

// Frame indices and expectations refer to the pre-transaction source.
// A missing replacement removes that frame; a present one replaces it.
struct Cine06FrameChange final {
    std::size_t index{};
    Cine06Frame expected;
    std::optional<Cine06Frame> replacement;
};

// Insert before the original frame index; before == frame count appends.
// For append, expected_next must be empty. For other insertions, it must
// contain the exact original frame at before.
struct Cine06FrameInsert final {
    std::size_t before{};
    std::optional<Cine06Frame> expected_next;
    Cine06Frame inserted;
};

struct Cine06Edits final {
    // Required for every frame insertion/replacement/deletion.
    std::optional<std::size_t> expected_frames;
    // Both fields are required for an optional default-wait edit.
    std::optional<std::uint16_t> expected_wait;
    std::optional<std::uint16_t> replacement_wait;
    std::vector<Cine06FrameChange> changes;
    std::vector<Cine06FrameInsert> inserts;
};

// Frame-level, source-preserving editor for any structurally valid ANM.
// The shared Cine00 codec validates all rebuilt chunks before a commit.
// Cine01/Cine02 remain the single command and image-state implementations.
class Cine06File final {
public:
    explicit Cine06File(std::vector<std::uint8_t> source);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] Cine06Frame frame(std::size_t index) const;
    [[nodiscard]] Cine01FrameInfo frame_info(std::size_t index) const;
    [[nodiscard]] std::vector<Cine01Cue> cues() const;
    [[nodiscard]] Cine02VisualState snapshot(std::size_t index) const;

    // A single transaction applies all original-indexed edits. Validation
    // rejects duplicate targets, stale frames, unknown/unsupported chunks,
    // frame-count overflow, and malformed replacement streams. On any error,
    // the original object remains byte-for-byte unchanged.
    void apply(const Cine06Edits& edits);

private:
    Cine02File source_;
};

} // namespace rloft
