#include "rloft/CINE05.hpp"

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
constexpr std::size_t kHeight = 256U;
constexpr std::size_t kPlaneBytes = 0x5000U;
constexpr std::size_t kPackedBytes = 0x14000U;
using ChunkKey = std::pair<std::size_t, std::size_t>;

std::size_t packed_offset(std::size_t x, std::size_t y) {
    return (x & 3U) * kPlaneBytes + y * 80U + (x >> 2U);
}
void validate_region(const Cine05RegionPatch& region) {
    if (region.x >= kWidth || region.y >= kHeight ||
        region.width == 0U || region.height == 0U ||
        region.width > kWidth - region.x ||
        region.height > kHeight - region.y)
        throw std::out_of_range("CINE05 region exceeds 320x256 viewport");
    const std::size_t area = static_cast<std::size_t>(region.width) * region.height;
    if (region.expected.size() != area || region.replacement.size() != area)
        throw std::invalid_argument("CINE05 region pixel buffer size mismatch");
}
struct Bounds final {
    std::size_t left = kWidth;
    std::size_t top = kHeight;
    std::size_t right{};
    std::size_t bottom{};
};
} // namespace

Cine05File::Cine05File(std::vector<std::uint8_t> source)
    : direct_(std::move(source)) {}
const CineStats& Cine05File::stats() const noexcept { return direct_.stats(); }
const std::vector<std::uint8_t>& Cine05File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Cine05File::encode() const { return direct_.encode(); }
CineChunkInfo Cine05File::info(std::size_t frame, std::size_t chunk) const {
    return direct_.info(frame, chunk);
}
std::vector<std::uint8_t> Cine05File::payload(
    std::size_t frame, std::size_t chunk) const {
    return direct_.payload(frame, chunk);
}
Cine02VisualState Cine05File::snapshot(std::size_t frame) const {
    return direct_.snapshot(frame);
}
Cine01FrameInfo Cine05File::frame_info(std::size_t frame) const {
    return direct_.frame_info(frame);
}
std::vector<Cine01Cue> Cine05File::cues() const { return direct_.cues(); }
std::uint8_t Cine05File::viewport_pixel(
    std::size_t frame, std::size_t chunk,
    std::uint16_t x, std::uint16_t y) const {
    return direct_.viewport_pixel(frame, chunk, x, y);
}

std::vector<Cine05RegionPatch> Cine05File::regions_to(
    const Cine05File& target) const {
    // The established byte-run comparator verifies all non-planar bytes
    // and source/target header and chunk layouts before we form rectangles.
    const auto runs = direct_.planar_runs_to(target.direct_);
    std::map<ChunkKey, Bounds> bounds;
    for (const auto& run : runs) {
        auto& box = bounds[{run.frame, run.chunk}];
        const auto first_x = static_cast<std::size_t>(run.byte_column) * 4U + run.plane;
        const auto last_x = first_x + (run.expected.size() - 1U) * 4U;
        box.left = std::min(box.left, first_x);
        box.right = std::max(box.right, last_x);
        box.top = std::min(box.top, static_cast<std::size_t>(run.row));
        box.bottom = std::max(box.bottom, static_cast<std::size_t>(run.row));
    }
    std::vector<Cine05RegionPatch> regions;
    regions.reserve(bounds.size());
    for (const auto& [key, box] : bounds) {
        Cine05RegionPatch region{};
        region.frame = key.first;
        region.chunk = key.second;
        region.x = static_cast<std::uint16_t>(box.left);
        region.y = static_cast<std::uint16_t>(box.top);
        region.width = static_cast<std::uint16_t>(box.right - box.left + 1U);
        region.height = static_cast<std::uint16_t>(box.bottom - box.top + 1U);
        const auto source = payload(key.first, key.second);
        const auto dest = target.payload(key.first, key.second);
        const auto area = static_cast<std::size_t>(region.width) * region.height;
        region.expected.reserve(area);
        region.replacement.reserve(area);
        for (std::size_t y = box.top; y <= box.bottom; ++y)
            for (std::size_t x = box.left; x <= box.right; ++x) {
                const auto offset = packed_offset(x, y);
                region.expected.push_back(source[offset]);
                region.replacement.push_back(dest[offset]);
            }
        regions.push_back(std::move(region));
    }
    return regions;
}

void Cine05File::apply(const Cine05Edits& edits) {
    Cine04Edits merged = edits.base;
    std::map<ChunkKey, std::vector<std::uint8_t>> modified;
    std::set<std::pair<ChunkKey, std::size_t>> occupied;
    for (const auto& region : edits.regions) {
        validate_region(region);
        const auto meta = info(region.frame, region.chunk);
        if (meta.id != 8U || meta.payload_bytes != kPackedBytes)
            throw std::invalid_argument("CINE05 region requires a native type-8 image");
        const ChunkKey key{region.frame, region.chunk};
        auto it = modified.find(key);
        if (it == modified.end())
            it = modified.emplace(key, payload(key.first, key.second)).first;
        auto& packet = it->second;
        for (std::size_t dy = 0U; dy < region.height; ++dy)
            for (std::size_t dx = 0U; dx < region.width; ++dx) {
                const auto index = dy * region.width + dx;
                const auto offset = packed_offset(region.x + dx, region.y + dy);
                if (!occupied.emplace(key, offset).second)
                    throw std::invalid_argument("CINE05 overlapping or duplicate region");
                if (packet[offset] != region.expected[index])
                    throw std::runtime_error("CINE05 stale region source pixel");
                packet[offset] = region.replacement[index];
            }
    }
    // Let Cine04/Cine03/Cine02/Cine01/Cine00 enforce duplicate packet
    // alias rejection with raw, planar-run or pixel edits and rebuild once.
    for (auto& [key, bytes] : modified)
        merged.base.base.base.packets.chunks.push_back(
            {key.first, key.second, payload(key.first, key.second),
             std::move(bytes)});
    Cine04File candidate(encode());
    candidate.apply(merged);
    direct_ = std::move(candidate);
}

} // namespace rloft
