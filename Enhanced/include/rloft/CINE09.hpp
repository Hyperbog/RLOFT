#pragma once

#include "rloft/CINE07.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace rloft {

// Native 320x400 chunky indices, 256 RGB triples in their original 6-bit
// VGA DAC form, and the timing/provenance of the source frame. Subtitles are
// commands, NOT composited glyphs: their fonts/text require runtime assets.
struct Cine09FrameSample final {
    std::size_t frame{};
    std::uint64_t start_tick{};
    std::uint16_t duration_ticks{};
    std::array<std::uint8_t, 768> dac6{};
    std::vector<std::uint8_t> pixels;
    std::optional<std::size_t> last_palette_frame;
    std::optional<std::size_t> last_image_frame;
    std::optional<std::size_t> active_subtitle_frame;
};

struct Cine09TimedCue final {
    Cine01Cue command;
    std::uint64_t start_tick{};
    std::uint16_t frame_duration_ticks{};
    bool subtitle_clear{};
};

// Time-addressable native screenshot / source-cue export for every CODE.2 ANM.
// Uses the previously validated CINE07 timeline and CINE02 pixel compositor.
// BMP output is lossless for palette *indices*; 6-bit VGA DAC values are
// expanded injectively to the 8-bit Windows BMP palette using (v<<2)|(v>>4).
// This is a declared display conversion, not a modification of the ANM file.
class Cine09File final {
public:
    explicit Cine09File(std::vector<std::uint8_t> source,
                        std::uint16_t alternate_image_wait = 6U);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] std::uint64_t total_ticks() const noexcept;
    [[nodiscard]] Cine09FrameSample sample_frame(std::size_t frame) const;
    [[nodiscard]] Cine09FrameSample sample_tick(std::uint64_t tick) const;
    [[nodiscard]] std::vector<Cine09TimedCue> timed_cues() const;
    [[nodiscard]] std::string cue_csv() const;

    // BITMAPFILEHEADER + 40-byte BITMAPINFOHEADER + 256 BGRA entries +
    // bottom-up, 4-byte-aligned native 320x400 pixel rows (8 bpp, BI_RGB).
    [[nodiscard]] static std::vector<std::uint8_t> encode_indexed_bmp(
        const Cine09FrameSample& sample);
    [[nodiscard]] std::vector<std::uint8_t> bmp_at_frame(
        std::size_t frame) const;
    [[nodiscard]] std::vector<std::uint8_t> bmp_at_tick(
        std::uint64_t tick) const;

    // Reuse source-checked, atomic original-frame range editing.
    void apply(const Cine07Edits& edits);

private:
    Cine07File source_;
};

} // namespace rloft
