#include "rloft/CINE08.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kPixels = 320U * 400U;
constexpr std::size_t kWidth = 320U;

void add16(std::vector<std::uint8_t>& out, std::uint16_t value) {
    out.push_back(static_cast<std::uint8_t>(value));
    out.push_back(static_cast<std::uint8_t>(value >> 8U));
}
void add32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    for (unsigned shift = 0U; shift < 32U; shift += 8U)
        out.push_back(static_cast<std::uint8_t>(value >> shift));
}
void packet(std::vector<std::uint8_t>& out, std::uint16_t id,
            const std::vector<std::uint8_t>& payload) {
    if (payload.size() > static_cast<std::size_t>(
            std::numeric_limits<std::int32_t>::max()))
        throw std::overflow_error("CINE08 packet exceeds signed 32-bit length");
    add16(out, id);
    add32(out, static_cast<std::uint32_t>(payload.size()));
    out.insert(out.end(), payload.begin(), payload.end());
}
} // namespace

Cine08File::Cine08File(std::vector<std::uint8_t> source,
                       std::uint16_t alternate_image_wait)
    : source_(std::move(source), alternate_image_wait) {}
const CineStats& Cine08File::stats() const noexcept { return source_.stats(); }
const std::vector<std::uint8_t>& Cine08File::bytes() const noexcept {
    return source_.bytes();
}
std::vector<std::uint8_t> Cine08File::encode() const {
    return source_.encode();
}
Cine02VisualState Cine08File::snapshot(std::size_t frame) const {
    return source_.snapshot(frame);
}
const Cine07FrameIndex& Cine08File::frame_index(std::size_t frame) const {
    return source_.frame_index(frame);
}
std::uint64_t Cine08File::total_ticks() const noexcept {
    return source_.total_ticks();
}

std::vector<std::uint8_t> Cine08File::active_subtitle(
    std::size_t before) const {
    const auto owner = frame_index(before).active_subtitle_frame;
    if (!owner) return {};
    const auto src = source_.frame(*owner);
    for (auto it = src.packets.rbegin(); it != src.packets.rend(); ++it) {
        if (it->id != 7U) continue;
        if (it->payload.size() != 4U)
            throw std::logic_error("CINE08 original subtitle packet changed");
        if (it->payload[0] == 0U && it->payload[1] == 0U)
            throw std::logic_error("CINE08 active subtitle has a clear command");
        return it->payload;
    }
    throw std::logic_error("CINE08 subtitle provenance has no command");
}

Cine08SegmentInfo Cine08File::segment_info(
    std::size_t first, std::size_t last) const {
    if (first >= last || last > stats().frames)
        throw std::out_of_range("CINE08 invalid segment frame range");
    const auto& first_index = frame_index(first);
    const auto final_tick = last == stats().frames
        ? total_ticks() : frame_index(last).start_tick;
    const bool subtitle = first > 0U &&
        frame_index(first - 1U).active_subtitle_frame.has_value();
    const std::size_t prefix = first == 0U ? 0U : (subtitle ? 3U : 2U);
    if (source_.frame(first).packets.size() >
        std::numeric_limits<std::uint16_t>::max() - prefix)
        throw std::overflow_error("CINE08 first frame has too many packets");
    return {first, last, prefix, subtitle,
            first_index.start_tick, final_tick - first_index.start_tick};
}

std::vector<std::uint8_t> Cine08File::encode_full_frame(
    const std::vector<std::uint8_t>& pixels) {
    if (pixels.size() != kPixels)
        throw std::invalid_argument("CINE08 RLE expects 320x400 indices");
    std::vector<std::uint8_t> result;
    result.reserve(400U * 323U);
    // Three literal runs per 320-pixel row: 128, 128, 64.
    for (std::size_t row = 0U; row < 400U; ++row) {
        for (std::size_t x = 0U; x < kWidth;) {
            const std::size_t n = kWidth - x > 128U ? 128U : kWidth - x;
            result.push_back(static_cast<std::uint8_t>(n - 1U));
            const auto begin = pixels.begin() +
                static_cast<std::ptrdiff_t>(row * kWidth + x);
            result.insert(result.end(), begin,
                begin + static_cast<std::ptrdiff_t>(n));
            x += n;
        }
    }
    return result;
}

std::vector<std::uint8_t> Cine08File::extract_visual_segment(
    std::size_t first, std::size_t last) const {
    const auto info = segment_info(first, last);
    if (first == 0U) return source_.extract_range(first, last);

    const auto before = snapshot(first - 1U);
    const std::vector<std::uint8_t> palette(
        before.palette.begin(), before.palette.end());
    const auto compressed = encode_full_frame(before.pixels);
    const auto text = info.restored_subtitle
        ? active_subtitle(first - 1U) : std::vector<std::uint8_t>{};
    const auto original_first = source_.frame_bytes(first, first + 1U);
    const auto untouched_tail = source_.frame_bytes(first + 1U, last);
    if (original_first.size() < 2U)
        throw std::logic_error("CINE08 first source frame has no chunk count");
    const auto original_count = source_.frame(first).packets.size();

    std::vector<std::uint8_t> result;
    result.reserve(4U + 6U * info.prefix_packets + palette.size() +
                   compressed.size() + text.size() + original_first.size() +
                   untouched_tail.size());
    add16(result, static_cast<std::uint16_t>(last - first));
    add16(result, stats().default_wait);
    add16(result, static_cast<std::uint16_t>(original_count + info.prefix_packets));
    packet(result, 0U, palette);
    packet(result, 2U, compressed);
    if (info.restored_subtitle) packet(result, 7U, text);
    result.insert(result.end(), original_first.begin() + 2, original_first.end());
    result.insert(result.end(), untouched_tail.begin(), untouched_tail.end());
    (void)Cine07File(result); // Canonical packet validator, before return.
    return result;
}
void Cine08File::apply(const Cine07Edits& edits) { source_.apply(edits); }

} // namespace rloft
