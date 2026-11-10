#pragma once

#include "rloft/CINE01.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// A deterministic image-layer snapshot after every packet through this frame.
// Screen is 320x400, row-major indexed pixels. DAC is 256 RGB triples using
// the original 6-bit values. Subtitles are intentionally NOT composited:
// text, fonts and audio are resolved by the existing AnmCinematic player.
struct Cine02VisualState final {
    std::size_t frame{};
    std::uint16_t wait_ticks{};
    std::array<std::uint8_t, 768> palette{};
    std::vector<std::uint8_t> pixels;
    std::optional<std::size_t> last_palette_frame;
    std::optional<std::size_t> last_image_frame;
};

// Edits one serialized VGA DAC component in an existing palette packet.
// Channel indexes are byte positions 0..767 (RGBRGB...). The original
// component and 6-bit replacement are checked before any data changes.
struct Cine02PalettePatch final {
    std::size_t frame{};
    std::size_t chunk{};
    std::size_t channel{};
    std::uint8_t expected{};
    std::uint8_t replacement{};
};

struct Cine02Edits final {
    Cine01Edits base; // Existing source-checked chunk and audiovisual edits.
    std::vector<Cine02PalettePatch> palette;
};

// Reuses Cine00's byte-exact packet codec and Cine01's typed cue timeline.
// Supports frame reconstruction for all CODE.2 image packets observed in the
// original corpus, including nonempty type-3 delta packets in CINE12.
class Cine02File final {
public:
    explicit Cine02File(std::vector<std::uint8_t> source);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] CineChunkInfo info(std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] std::vector<std::uint8_t> payload(
        std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] std::vector<Cine01Cue> cues() const;
    [[nodiscard]] Cine01FrameInfo frame_info(std::size_t frame) const;

    [[nodiscard]] Cine02VisualState snapshot(std::size_t frame) const;
    void apply(const Cine02Edits& edits);

private:
    Cine01File direct_;
};

} // namespace rloft
