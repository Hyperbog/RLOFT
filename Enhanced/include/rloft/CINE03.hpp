#pragma once

#include "rloft/CINE02.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Type-8 VGA images are serialized as four 0x5000-byte planes. Coordinates
// refer to the unscaled 320x256 viewport (screen y=51..306), NOT the
// surrounding 320x400 framebuffer. A source byte is checked before writing.
struct Cine03PixelPatch final {
    std::size_t frame{};
    std::size_t chunk{};
    std::uint16_t x{};
    std::uint16_t y{};
    std::uint8_t expected{};
    std::uint8_t replacement{};
};

struct Cine03Edits final {
    Cine02Edits base; // Existing checked palette, cue, packet and timing edits.
    std::vector<Cine03PixelPatch> pixels;
};

// A reusable source-backed editor for related ANM variants. CINE02 and
// CINE03 share every header, command and packet except 2,534 planar pixels
// across 34 image packets. The APIs are not hard-coded to those two files.
class Cine03File final {
public:
    explicit Cine03File(std::vector<std::uint8_t> source);

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

    // Return every changed native pixel needed to transform *this into
    // target. Require IDENTICAL frame/packet layouts, header timing and all
    // non-type-8 payload bytes; otherwise refuse a pixel-only migration.
    [[nodiscard]] std::vector<Cine03PixelPatch> pixel_differences(
        const Cine03File& target) const;

    // Reject duplicate, stale, off-map or non-image edits, including aliasing
    // with lower-level packet edits. Stage all changes through the canonical
    // Cine02 -> Cine01 -> Cine00 serializer; commit only on full validation.
    void apply(const Cine03Edits& edits);

private:
    Cine02File direct_;
};

} // namespace rloft
