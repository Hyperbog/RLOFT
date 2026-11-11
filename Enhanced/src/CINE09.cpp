#include "rloft/CINE09.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kWidth = 320U;
constexpr std::size_t kHeight = 400U;
constexpr std::size_t kPaletteEntries = 256U;
constexpr std::size_t kBmpHeader = 14U + 40U;
constexpr std::size_t kBmpPixelsOffset = kBmpHeader + kPaletteEntries * 4U;
constexpr std::size_t kPixels = kWidth * kHeight;
constexpr std::size_t kBmpSize = kBmpPixelsOffset + kPixels;

void put16(std::vector<std::uint8_t>& bytes, std::uint16_t value) {
    bytes.push_back(static_cast<std::uint8_t>(value));
    bytes.push_back(static_cast<std::uint8_t>(value >> 8U));
}
void put32(std::vector<std::uint8_t>& bytes, std::uint32_t value) {
    for (unsigned shift = 0U; shift < 32U; shift += 8U)
        bytes.push_back(static_cast<std::uint8_t>(value >> shift));
}
std::uint8_t expand_dac6(std::uint8_t original) {
    if (original > 63U)
        throw std::runtime_error("CINE09 palette component exceeds VGA DAC6 domain");
    return static_cast<std::uint8_t>((original << 2U) | (original >> 4U));
}
} // namespace

Cine09File::Cine09File(std::vector<std::uint8_t> source,
                       std::uint16_t alternate_image_wait)
    : source_(std::move(source), alternate_image_wait) {}

const CineStats& Cine09File::stats() const noexcept { return source_.stats(); }
const std::vector<std::uint8_t>& Cine09File::bytes() const noexcept {
    return source_.bytes();
}
std::vector<std::uint8_t> Cine09File::encode() const {
    return source_.encode();
}
std::uint64_t Cine09File::total_ticks() const noexcept {
    return source_.total_ticks();
}

Cine09FrameSample Cine09File::sample_frame(std::size_t frame) const {
    const auto image = source_.snapshot(frame);
    const auto& position = source_.frame_index(frame);
    if (image.pixels.size() != kPixels)
        throw std::logic_error("CINE09 canonical compositor returned wrong screen size");
    return {frame, position.start_tick, position.duration_ticks,
            image.palette, image.pixels, image.last_palette_frame,
            image.last_image_frame, position.active_subtitle_frame};
}

Cine09FrameSample Cine09File::sample_tick(std::uint64_t tick) const {
    return sample_frame(source_.seek_tick(tick).frame.frame);
}

std::vector<Cine09TimedCue> Cine09File::timed_cues() const {
    const auto commands = source_.cues();
    std::vector<Cine09TimedCue> result;
    result.reserve(commands.size());
    for (const auto& command : commands) {
        const auto& index = source_.frame_index(command.frame);
        result.push_back({command, index.start_tick, index.duration_ticks,
                          command.kind == Cine01CueKind::Subtitle &&
                          command.first == 0U});
    }
    return result;
}

std::string Cine09File::cue_csv() const {
    std::ostringstream csv;
    csv << "frame,chunk,start_tick,frame_duration_ticks,type,first,second,"
           "queue_suppressed,subtitle_clear\n";
    for (const auto& item : timed_cues()) {
        const auto& cue = item.command;
        csv << cue.frame << ',' << cue.chunk << ',' << item.start_tick << ','
            << item.frame_duration_ticks << ','
            << static_cast<std::uint16_t>(cue.kind) << ',' << cue.first << ','
            << cue.second << ',' << (cue.queue_suppressed ? 1 : 0) << ','
            << (item.subtitle_clear ? 1 : 0) << '\n';
    }
    return csv.str();
}

std::vector<std::uint8_t> Cine09File::encode_indexed_bmp(
    const Cine09FrameSample& sample) {
    if (sample.pixels.size() != kPixels)
        throw std::invalid_argument("CINE09 indexed BMP requires 320x400 pixels");
    static_assert(kBmpSize <= std::numeric_limits<std::uint32_t>::max(),
                  "CINE09 BMP size exceeds uint32");
    std::vector<std::uint8_t> bmp;
    bmp.reserve(kBmpSize);
    bmp.push_back('B'); bmp.push_back('M');
    put32(bmp, static_cast<std::uint32_t>(kBmpSize));
    put16(bmp, 0U); put16(bmp, 0U);
    put32(bmp, static_cast<std::uint32_t>(kBmpPixelsOffset));
    put32(bmp, 40U); // BITMAPINFOHEADER
    put32(bmp, static_cast<std::uint32_t>(kWidth));
    put32(bmp, static_cast<std::uint32_t>(kHeight)); // positive -> bottom-up
    put16(bmp, 1U); // one plane
    put16(bmp, 8U); // indexed 8 bpp
    put32(bmp, 0U); // BI_RGB uncompressed
    put32(bmp, static_cast<std::uint32_t>(kPixels));
    put32(bmp, 0U); put32(bmp, 0U); // physical resolution unspecified
    put32(bmp, static_cast<std::uint32_t>(kPaletteEntries));
    put32(bmp, 0U); // all palette entries may be used
    if (bmp.size() != kBmpHeader)
        throw std::logic_error("CINE09 BMP header size mismatch");

    for (std::size_t i = 0U; i < kPaletteEntries; ++i) {
        const auto red = expand_dac6(sample.dac6[i * 3U]);
        const auto green = expand_dac6(sample.dac6[i * 3U + 1U]);
        const auto blue = expand_dac6(sample.dac6[i * 3U + 2U]);
        bmp.insert(bmp.end(), {blue, green, red, 0U});
    }
    if (bmp.size() != kBmpPixelsOffset)
        throw std::logic_error("CINE09 BMP palette offset mismatch");
    // 320 bytes/scanline is already 4-byte aligned (no row padding).
    for (std::size_t row = kHeight; row != 0U; --row) {
        const auto first = sample.pixels.begin() +
            static_cast<std::ptrdiff_t>((row - 1U) * kWidth);
        bmp.insert(bmp.end(), first, first + static_cast<std::ptrdiff_t>(kWidth));
    }
    if (bmp.size() != kBmpSize)
        throw std::logic_error("CINE09 BMP final size mismatch");
    return bmp;
}

std::vector<std::uint8_t> Cine09File::bmp_at_frame(std::size_t frame) const {
    return encode_indexed_bmp(sample_frame(frame));
}
std::vector<std::uint8_t> Cine09File::bmp_at_tick(std::uint64_t tick) const {
    return encode_indexed_bmp(sample_tick(tick));
}

void Cine09File::apply(const Cine07Edits& edits) {
    source_.apply(edits);
}

} // namespace rloft
