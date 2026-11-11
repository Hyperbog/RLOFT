#include "rloft/CINE04.hpp"

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
constexpr std::size_t kPlaneBytes = 0x5000U;
constexpr std::size_t kPackedBytes = 0x14000U;
constexpr std::size_t kByteColumns = 80U;
constexpr std::size_t kRows = 256U;
using ChunkKey = std::pair<std::size_t, std::size_t>;

std::size_t run_offset(const Cine04PlanarRun& run) {
    if (run.plane >= 4U || run.row >= kRows ||
        run.byte_column >= kByteColumns || run.expected.empty() ||
        run.expected.size() != run.replacement.size() ||
        run.expected.size() > kByteColumns - run.byte_column)
        throw std::invalid_argument("CINE04 malformed or cross-row planar run");
    return static_cast<std::size_t>(run.plane) * kPlaneBytes +
           static_cast<std::size_t>(run.row) * kByteColumns + run.byte_column;
}
} // namespace

Cine04File::Cine04File(std::vector<std::uint8_t> source)
    : direct_(std::move(source)) {}
const CineStats& Cine04File::stats() const noexcept { return direct_.stats(); }
const std::vector<std::uint8_t>& Cine04File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Cine04File::encode() const {
    return direct_.encode();
}
CineChunkInfo Cine04File::info(std::size_t frame, std::size_t chunk) const {
    return direct_.info(frame, chunk);
}
std::vector<std::uint8_t> Cine04File::payload(
    std::size_t frame, std::size_t chunk) const {
    return direct_.payload(frame, chunk);
}
Cine02VisualState Cine04File::snapshot(std::size_t frame) const {
    return direct_.snapshot(frame);
}
Cine01FrameInfo Cine04File::frame_info(std::size_t frame) const {
    return direct_.frame_info(frame);
}
std::vector<Cine01Cue> Cine04File::cues() const { return direct_.cues(); }
std::uint8_t Cine04File::viewport_pixel(
    std::size_t frame, std::size_t chunk,
    std::uint16_t x, std::uint16_t y) const {
    return direct_.viewport_pixel(frame, chunk, x, y);
}

std::vector<Cine04PlanarRun> Cine04File::planar_runs_to(
    const Cine04File& target) const {
    // The canonical Cine03 checker certifies that *only* planar pixels differ.
    const auto pixels = direct_.pixel_differences(target.direct_);
    std::vector<Cine04PlanarRun> result;
    result.reserve(pixels.size());
    for (const auto& pixel : pixels) {
        const auto plane = static_cast<std::uint8_t>(pixel.x & 3U);
        const auto column = static_cast<std::uint8_t>(pixel.x >> 2U);
        if (!result.empty()) {
            auto& last = result.back();
            if (last.frame == pixel.frame && last.chunk == pixel.chunk &&
                last.plane == plane && last.row == pixel.y &&
                static_cast<std::size_t>(last.byte_column) +
                    last.expected.size() == column) {
                last.expected.push_back(pixel.expected);
                last.replacement.push_back(pixel.replacement);
                continue;
            }
        }
        result.push_back({pixel.frame, pixel.chunk, plane, pixel.y,
                          column, {pixel.expected}, {pixel.replacement}});
    }
    return result;
}

void Cine04File::apply(const Cine04Edits& edits) {
    Cine03Edits merged = edits.base;
    std::map<ChunkKey, std::vector<std::uint8_t>> modified;
    std::set<std::pair<ChunkKey, std::size_t>> occupied;
    for (const auto& run : edits.runs) {
        const auto offset = run_offset(run);
        const auto metadata = info(run.frame, run.chunk);
        if (metadata.id != 8U || metadata.payload_bytes != kPackedBytes)
            throw std::invalid_argument("CINE04 run targets a non-planar packet");
        const ChunkKey key{run.frame, run.chunk};
        auto it = modified.find(key);
        if (it == modified.end())
            it = modified.emplace(key, payload(run.frame, run.chunk)).first;
        auto& packet = it->second;
        // Compare the source before any writes. Overlaps are rejected even if
        // two different runs would assign the same replacement byte.
        for (std::size_t i = 0U; i < run.expected.size(); ++i) {
            if (!occupied.emplace(key, offset + i).second)
                throw std::invalid_argument("CINE04 overlapping planar runs");
            if (packet[offset + i] != run.expected[i])
                throw std::runtime_error("CINE04 stale planar run source");
        }
        std::copy(run.replacement.begin(), run.replacement.end(),
                  packet.begin() + static_cast<std::ptrdiff_t>(offset));
    }
    // Existing Cine00 writer validates duplicate packet aliases, including
    // overlaps with typed CINE03 pixel edits, and rebuilt packet boundaries.
    for (auto& [key, packet] : modified)
        merged.base.base.packets.chunks.push_back(
            {key.first, key.second, payload(key.first, key.second),
             std::move(packet)});
    Cine03File candidate(encode());
    candidate.apply(merged);
    direct_ = std::move(candidate);
}

} // namespace rloft
