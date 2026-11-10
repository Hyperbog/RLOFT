#include "rloft/CINE03.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kViewportWidth = 320U;
constexpr std::size_t kViewportHeight = 256U;
constexpr std::size_t kPlaneBytes = 0x5000U;
constexpr std::size_t kPackedBytes = 0x14000U;
using ChunkKey = std::pair<std::size_t, std::size_t>;

std::size_t packed_offset(std::uint16_t x, std::uint16_t y) {
    if (x >= kViewportWidth || y >= kViewportHeight)
        throw std::out_of_range("CINE03 pixel outside native viewport");
    return (static_cast<std::size_t>(x) & 3U) * kPlaneBytes +
           static_cast<std::size_t>(y) * 80U + (x >> 2U);
}

void require_planar(const CineChunkInfo& info) {
    if (info.id != 8U || info.payload_bytes != kPackedBytes)
        throw std::invalid_argument("CINE03 edit requires a type-8 viewport");
}

} // namespace

Cine03File::Cine03File(std::vector<std::uint8_t> source)
    : direct_(std::move(source)) {}
const CineStats& Cine03File::stats() const noexcept { return direct_.stats(); }
const std::vector<std::uint8_t>& Cine03File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Cine03File::encode() const { return direct_.encode(); }
CineChunkInfo Cine03File::info(std::size_t frame, std::size_t chunk) const {
    return direct_.info(frame, chunk);
}
std::vector<std::uint8_t> Cine03File::payload(
    std::size_t frame, std::size_t chunk) const {
    return direct_.payload(frame, chunk);
}
Cine02VisualState Cine03File::snapshot(std::size_t frame) const {
    return direct_.snapshot(frame);
}
Cine01FrameInfo Cine03File::frame_info(std::size_t frame) const {
    return direct_.frame_info(frame);
}
std::vector<Cine01Cue> Cine03File::cues() const { return direct_.cues(); }

std::uint8_t Cine03File::viewport_pixel(
    std::size_t frame, std::size_t chunk,
    std::uint16_t x, std::uint16_t y) const {
    require_planar(info(frame, chunk));
    return payload(frame, chunk)[packed_offset(x, y)];
}

std::vector<Cine03PixelPatch> Cine03File::pixel_differences(
    const Cine03File& target) const {
    if (stats().frames != target.stats().frames ||
        stats().default_wait != target.stats().default_wait)
        throw std::invalid_argument("CINE03 variants have different frame counts or timing");
    std::vector<Cine03PixelPatch> changes;
    for (std::size_t frame = 0U; frame < stats().frames; ++frame) {
        const auto count = frame_info(frame).chunks;
        if (count != target.frame_info(frame).chunks)
            throw std::invalid_argument("CINE03 variants have different chunk counts");
        for (std::size_t chunk = 0U; chunk < count; ++chunk) {
            const auto first = info(frame, chunk);
            const auto second = target.info(frame, chunk);
            if (first.id != second.id ||
                first.payload_bytes != second.payload_bytes)
                throw std::invalid_argument("CINE03 variants have different chunk layout");
            const auto original = payload(frame, chunk);
            const auto replacement = target.payload(frame, chunk);
            if (first.id != 8U) {
                if (original != replacement)
                    throw std::invalid_argument(
                        "CINE03 variants differ outside native planar images");
                continue;
            }
            require_planar(first);
            for (std::size_t offset = 0U; offset < original.size(); ++offset) {
                if (original[offset] == replacement[offset]) continue;
                const std::size_t plane = offset / kPlaneBytes;
                const std::size_t linear = offset % kPlaneBytes;
                changes.push_back({frame, chunk,
                    static_cast<std::uint16_t>((linear % 80U) * 4U + plane),
                    static_cast<std::uint16_t>(linear / 80U),
                    original[offset], replacement[offset]});
            }
        }
    }
    return changes;
}

void Cine03File::apply(const Cine03Edits& edits) {
    Cine02Edits merged = edits.base;
    std::map<ChunkKey, std::vector<std::uint8_t>> packets;
    std::set<std::tuple<std::size_t, std::size_t, std::size_t>> used_pixels;
    for (const auto& patch : edits.pixels) {
        const ChunkKey key{patch.frame, patch.chunk};
        require_planar(info(patch.frame, patch.chunk));
        const auto offset = packed_offset(patch.x, patch.y);
        if (!used_pixels.emplace(patch.frame, patch.chunk, offset).second)
            throw std::invalid_argument("CINE03 duplicate pixel edit");
        auto found = packets.find(key);
        if (found == packets.end())
            found = packets.emplace(key, payload(patch.frame, patch.chunk)).first;
        // Compare the expected value to the pre-transaction image rather
        // than an already staged pixel, preventing order-dependent edits.
        if (found->second[offset] != patch.expected)
            throw std::runtime_error("CINE03 stale image pixel");
        found->second[offset] = patch.replacement;
    }
    for (auto& [key, replacement] : packets)
        merged.base.packets.chunks.push_back({key.first, key.second,
            payload(key.first, key.second), std::move(replacement)});
    Cine02File candidate(encode());
    candidate.apply(merged);
    direct_ = std::move(candidate);
}

} // namespace rloft
