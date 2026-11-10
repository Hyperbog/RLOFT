#include "rloft/CINE02.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kWidth = 320U;
constexpr std::size_t kHeight = 400U;
constexpr std::size_t kViewportStart = 51U;
constexpr std::size_t kViewportHeight = 256U;

std::uint16_t next_word(const std::vector<std::uint8_t>& data,
                        std::size_t& pos) {
    if (pos > data.size() || data.size() - pos < 2U)
        throw std::runtime_error("CINE02 truncated image delta word");
    const auto value = static_cast<std::uint16_t>(
        data[pos] | (static_cast<unsigned>(data[pos + 1U]) << 8U));
    pos += 2U;
    return value;
}

// CODE.2 27AEC: planar address selects x&3; after unpacking to chunky
// pixels the same command updates a single screen column at each y.
void composite_delta(std::vector<std::uint8_t>& pixels,
                     const std::vector<std::uint8_t>& packet) {
    std::size_t pos = 0U;
    const auto count = next_word(packet, pos);
    std::size_t x = 0U;
    for (std::size_t i = 0; i < count; ++i) {
        x += next_word(packet, pos);
        const auto groups = next_word(packet, pos);
        if (x >= kWidth)
            throw std::runtime_error("CINE02 delta column outside screen");
        std::size_t y = 0U;
        for (std::size_t group = 0; group < groups; ++group) {
            y += next_word(packet, pos);
            const auto length = next_word(packet, pos);
            if (y > kHeight || length > kHeight - y ||
                pos > packet.size() || length > packet.size() - pos)
                throw std::runtime_error("CINE02 delta run outside screen");
            for (std::size_t n = 0; n < length; ++n)
                pixels[(y + n) * kWidth + x] = packet[pos + n];
            y += length;
            pos += length;
        }
    }
    if (pos != packet.size())
        throw std::runtime_error("CINE02 delta has trailing bytes");
}
} // namespace

Cine02File::Cine02File(std::vector<std::uint8_t> source)
    : direct_(std::move(source)) {}
const CineStats& Cine02File::stats() const noexcept { return direct_.stats(); }
const std::vector<std::uint8_t>& Cine02File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Cine02File::encode() const { return direct_.encode(); }
CineChunkInfo Cine02File::info(std::size_t frame, std::size_t chunk) const {
    return direct_.info(frame, chunk);
}
std::vector<std::uint8_t> Cine02File::payload(
    std::size_t frame, std::size_t chunk) const {
    return direct_.payload(frame, chunk);
}
std::vector<Cine01Cue> Cine02File::cues() const { return direct_.cues(); }
Cine01FrameInfo Cine02File::frame_info(std::size_t frame) const {
    return direct_.frame_info(frame);
}

Cine02VisualState Cine02File::snapshot(std::size_t target) const {
    if (target >= stats().frames)
        throw std::out_of_range("CINE02 snapshot frame index");
    Cine02VisualState state{};
    state.frame = target;
    state.wait_ticks = frame_info(target).wait_ticks;
    state.pixels.assign(kWidth * kHeight, 0U);
    for (std::size_t frame = 0U; frame <= target; ++frame) {
        const auto count = direct_.chunk_count(frame);
        for (std::size_t chunk = 0U; chunk < count; ++chunk) {
            const auto id = info(frame, chunk).id;
            switch (id) {
                case 0U: {
                    const auto p = payload(frame, chunk);
                    std::copy(p.begin(), p.end(), state.palette.begin());
                    state.last_palette_frame = frame;
                    break;
                }
                case 1U:
                    std::fill(state.pixels.begin(), state.pixels.end(), 0U);
                    state.palette.fill(0U);
                    state.last_palette_frame = frame;
                    state.last_image_frame = frame;
                    break;
                case 2U:
                    state.pixels = direct_.full_frame(frame, chunk);
                    state.last_image_frame = frame;
                    break;
                case 3U: {
                    const auto delta = payload(frame, chunk);
                    composite_delta(state.pixels, delta);
                    if (delta.size() > 2U) state.last_image_frame = frame;
                    break;
                }
                case 8U: {
                    const auto image = direct_.viewport(frame, chunk);
                    for (std::size_t y = 0U; y < kViewportHeight; ++y)
                        std::copy_n(image.begin() +
                                static_cast<std::ptrdiff_t>(y * kWidth),
                            kWidth, state.pixels.begin() +
                                static_cast<std::ptrdiff_t>((y + kViewportStart) * kWidth));
                    state.last_image_frame = frame;
                    break;
                }
                default: break; // Audio/text handled by the playback engine.
            }
        }
    }
    return state;
}

void Cine02File::apply(const Cine02Edits& edits) {
    Cine01Edits merged = edits.base;
    using ChunkKey = std::pair<std::size_t, std::size_t>;
    std::map<ChunkKey, std::vector<std::uint8_t>> replacements;
    std::set<std::pair<ChunkKey, std::size_t>> seen_channels;
    for (const auto& change : edits.palette) {
        const ChunkKey key{change.frame, change.chunk};
        if (info(change.frame, change.chunk).id != 0U)
            throw std::invalid_argument("CINE02 edit targets non-palette packet");
        if (change.channel >= 768U || change.replacement > 63U)
            throw std::out_of_range("CINE02 invalid 6-bit DAC component");
        if (!seen_channels.emplace(key, change.channel).second)
            throw std::invalid_argument("CINE02 duplicate palette component");
        auto it = replacements.find(key);
        if (it == replacements.end())
            it = replacements.emplace(key,
                payload(change.frame, change.chunk)).first;
        const auto original = payload(change.frame, change.chunk);
        if (original[change.channel] != change.expected)
            throw std::runtime_error("CINE02 stale palette component");
        it->second[change.channel] = change.replacement;
    }
    for (auto& pair : replacements)
        merged.packets.chunks.push_back({pair.first.first, pair.first.second,
            payload(pair.first.first, pair.first.second), std::move(pair.second)});
    Cine01File candidate(encode());
    candidate.apply(merged); // Duplicate target and all payload validation.
    direct_ = std::move(candidate); // Commit after entire transaction passes.
}

} // namespace rloft
