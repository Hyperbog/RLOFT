#pragma once

#include "rloft/CINE03.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// A contiguous run in the *original* CODE.2 type-8 VGA plane layout.
// Four 0x5000-byte planes, 80 bytes per scanline, 256 scanlines.
// plane=0..3, row=0..255, byte_column=0..79; run cannot cross a row.
// Each byte edits one indexed pixel at x=(byte_column+i)*4+plane.
struct Cine04PlanarRun final {
    std::size_t frame{};
    std::size_t chunk{};
    std::uint8_t plane{};
    std::uint16_t row{};
    std::uint8_t byte_column{};
    std::vector<std::uint8_t> expected;
    std::vector<std::uint8_t> replacement;
};

struct Cine04Edits final {
    Cine03Edits base; // Existing atomic palette, cue, timing and pixel edits.
    std::vector<Cine04PlanarRun> runs;
};

// Efficient source-checked editing across the original CINE02/03/04 family.
// Compares through Cine03's validated generic variant checker; packs only
// adjacent changed bytes within the SAME original VGA plane and scanline.
// The engine's existing ANM parser and playback paths remain unchanged.
class Cine04File final {
public:
    explicit Cine04File(std::vector<std::uint8_t> source);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] CineChunkInfo info(std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] std::vector<std::uint8_t> payload(
        std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] Cine02VisualState snapshot(std::size_t frame) const;
    [[nodiscard]] Cine01FrameInfo frame_info(std::size_t frame) const;
    [[nodiscard]] std::vector<Cine01Cue> cues() const;
    [[nodiscard]] std::uint8_t viewport_pixel(
        std::size_t frame, std::size_t chunk,
        std::uint16_t x, std::uint16_t y) const;

    // Rejects differences in frame structure, header timing, palette,
    // subtitle, audio or non-planar packets; emits deterministic native runs.
    [[nodiscard]] std::vector<Cine04PlanarRun> planar_runs_to(
        const Cine04File& target) const;

    // Requires an exact preimage, refuses overlapping runs and duplicate
    // aliases to Cine03 pixel/raw edits, then validates a rebuilt candidate
    // before committing. Invalid edits cannot modify the input object.
    void apply(const Cine04Edits& edits);

private:
    Cine03File direct_;
};

} // namespace rloft
