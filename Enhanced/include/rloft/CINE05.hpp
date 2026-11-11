#pragma once

#include "rloft/CINE04.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// A rectangular region of indexed pixels in a CODE.2 type-8 320x256
// viewport. Coordinates are viewport-relative (screen y=51..306).
// Source and replacement pixels are row-major, exactly width*height bytes.
// The file's four native VGA planes remain unchanged outside the region.
struct Cine05RegionPatch final {
    std::size_t frame{};
    std::size_t chunk{};
    std::uint16_t x{};
    std::uint16_t y{};
    std::uint16_t width{};
    std::uint16_t height{};
    std::vector<std::uint8_t> expected;
    std::vector<std::uint8_t> replacement;
};

struct Cine05Edits final {
    Cine04Edits base; // Existing planar runs, pixels, palette, cues, timing.
    std::vector<Cine05RegionPatch> regions;
};

// Works for any compatible CINE*.ANM, not just CINE05. Computes one minimal
// bounding rectangle per differing type-8 packet. A region may include
// unchanged pixels between changed ones, always checked against source.
// No new binary parser or speculative subtitle/audio behavior is introduced.
class Cine05File final {
public:
    explicit Cine05File(std::vector<std::uint8_t> source);

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

    // Refuses different headers, packet layouts or any non-planar change.
    // Regions cover all changed pixels and are deterministic in frame order.
    [[nodiscard]] std::vector<Cine05RegionPatch> regions_to(
        const Cine05File& target) const;

    // Rejects stale, duplicate, overlapping, out-of-bounds and non-planar
    // regions. A region may not alias a lower-level edit of the same packet.
    // The complete candidate is validated before the source is replaced.
    void apply(const Cine05Edits& edits);

private:
    Cine04File direct_;
};

} // namespace rloft
