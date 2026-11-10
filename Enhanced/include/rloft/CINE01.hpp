#pragma once

#include "rloft/CINE00.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// CODE.2 cinematic control packet IDs. These are packet types, not inferred
// executable side effects; source parameter words are preserved as unsigned.
enum class Cine01CueKind : std::uint16_t {
    MusicSequence = 5,
    QueuedSound = 6,
    Subtitle = 7,
    DirectSound = 9
};

struct Cine01Cue final {
    std::size_t frame{};
    std::size_t chunk{};
    Cine01CueKind kind{};
    std::uint16_t first{};
    std::uint16_t second{}; // Zero for 2-byte commands (6, 9).
    bool queue_suppressed{}; // CODE.2 skips queued IDs 3, 15, 0x88.
};

struct Cine01FrameInfo final {
    std::size_t frame{};
    std::size_t chunks{};
    bool uses_image_wait{}; // Type 3 or 8 selects the alternate wait.
    std::uint16_t wait_ticks{}; // For CINE01: 6 if alternate, else header wait.
};

// One source-checked replacement of a command's two serialized words (or
// single word for types 6 and 9). The original kind and packet ID never move.
struct Cine01CuePatch final {
    std::size_t frame{};
    std::size_t chunk{};
    Cine01CueKind expected_kind{};
    std::uint16_t expected_first{};
    std::uint16_t expected_second{};
    std::uint16_t replacement_first{};
    std::uint16_t replacement_second{};
};

struct Cine01Edits final {
    CineEdits packets; // Existing palette, RLE, planar and wait edits.
    std::vector<Cine01CuePatch> cues;
};

// CINE01 adds a typed, frame-indexed audiovisual timeline to the existing
// source-preserving Cine00File codec. It never reparses image payloads or
// reinterprets text/sound IDs as external archive indices.
class Cine01File final {
public:
    explicit Cine01File(std::vector<std::uint8_t> source);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] std::size_t chunk_count(std::size_t frame) const;
    [[nodiscard]] CineChunkInfo info(std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] std::vector<std::uint8_t> payload(
        std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] std::vector<std::uint8_t> full_frame(
        std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] std::vector<std::uint8_t> viewport(
        std::size_t frame, std::size_t chunk) const;

    [[nodiscard]] Cine01FrameInfo frame_info(std::size_t frame) const;
    [[nodiscard]] Cine01Cue cue(std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] std::vector<Cine01Cue> cues() const;
    [[nodiscard]] std::vector<Cine01Cue> cues_at(std::size_t frame) const;

    // Merge raw packet and typed cue edits. The original codec checks for
    // duplicate targets, stale sources, invalid payloads and performs the
    // single all-or-nothing archive rebuild after all edits are validated.
    void apply(const Cine01Edits& edits);

private:
    Cine00File direct_;
};

} // namespace rloft
