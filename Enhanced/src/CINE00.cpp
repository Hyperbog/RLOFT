#include "rloft/CINE00.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kWidth = 320U;
constexpr std::size_t kHeight = 400U;
constexpr std::size_t kViewportHeight = 256U;
constexpr std::size_t kPlaneBytes = 0x5000U;
constexpr std::size_t kViewportBytes = 0x14000U;

std::uint16_t u16(const std::vector<std::uint8_t>& b, std::size_t& at) {
    if (at > b.size() || b.size() - at < 2U)
        throw std::runtime_error("ANM truncated uint16");
    const auto v = static_cast<std::uint16_t>(
        b[at] | (static_cast<unsigned>(b[at + 1U]) << 8U));
    at += 2U;
    return v;
}
std::uint32_t u32(const std::vector<std::uint8_t>& b, std::size_t& at) {
    if (at > b.size() || b.size() - at < 4U)
        throw std::runtime_error("ANM truncated chunk size");
    const auto v = static_cast<std::uint32_t>(b[at]) |
        (static_cast<std::uint32_t>(b[at + 1U]) << 8U) |
        (static_cast<std::uint32_t>(b[at + 2U]) << 16U) |
        (static_cast<std::uint32_t>(b[at + 3U]) << 24U);
    at += 4U;
    return v;
}
void append16(std::vector<std::uint8_t>& out, std::uint16_t x) {
    out.push_back(static_cast<std::uint8_t>(x));
    out.push_back(static_cast<std::uint8_t>(x >> 8U));
}
void append32(std::vector<std::uint8_t>& out, std::uint32_t x) {
    for (unsigned i = 0U; i < 4U; ++i)
        out.push_back(static_cast<std::uint8_t>(x >> (8U * i)));
}
std::vector<std::uint8_t> decode_rle(const std::vector<std::uint8_t>& b) {
    std::vector<std::uint8_t> pixels(kWidth * kHeight);
    std::size_t src = 0U;
    for (std::size_t y = 0U; y < kHeight; ++y) {
        std::size_t x = 0U;
        while (x < kWidth) {
            if (src >= b.size())
                throw std::runtime_error("ANM type-2 stream ends inside row");
            const auto op = b[src++];
            const std::size_t count = op < 0x80U
                ? static_cast<std::size_t>(op) + 1U : 0x101U - op;
            if (count > kWidth - x)
                throw std::runtime_error("ANM type-2 run crosses row");
            auto dest = pixels.begin() + static_cast<std::ptrdiff_t>(y * kWidth + x);
            if (op < 0x80U) {
                if (count > b.size() - src)
                    throw std::runtime_error("ANM type-2 truncated literals");
                std::copy_n(b.begin() + static_cast<std::ptrdiff_t>(src),
                            static_cast<std::ptrdiff_t>(count), dest);
                src += count;
            } else {
                if (src >= b.size())
                    throw std::runtime_error("ANM type-2 missing repeat pixel");
                std::fill_n(dest, static_cast<std::ptrdiff_t>(count), b[src++]);
            }
            x += count;
        }
    }
    if (src != b.size())
        throw std::runtime_error("ANM type-2 has trailing data");
    return pixels;
}
void validate_delta(const std::vector<std::uint8_t>& b) {
    std::size_t at = 0U;
    const auto outer = u16(b, at);
    std::size_t x = 0U;
    for (std::size_t i = 0; i < outer; ++i) {
        x += u16(b, at);
        const auto groups = u16(b, at);
        if (x >= kWidth)
            throw std::runtime_error("ANM type-3 x exceeds native viewport");
        std::size_t y = 0U;
        for (std::size_t group = 0; group < groups; ++group) {
            y += u16(b, at);
            const auto run = u16(b, at);
            if (y > kHeight || run > kHeight - y ||
                at > b.size() || run > b.size() - at)
                throw std::runtime_error("ANM type-3 truncated or out-of-range run");
            at += run;
            y += run;
        }
    }
    if (at != b.size())
        throw std::runtime_error("ANM type-3 has trailing data");
}
void validate_payload(std::uint16_t id, const std::vector<std::uint8_t>& p) {
    switch (id) {
        case 0: if (p.size() != 768U) throw std::runtime_error("ANM palette size"); break;
        case 1: if (!p.empty()) throw std::runtime_error("ANM blackout size"); break;
        case 2: (void)decode_rle(p); break;
        case 3: validate_delta(p); break;
        case 4: break; // Unobserved seek/repeat chunk: preserve without interpretation.
        case 5: case 7: if (p.size() != 4U) throw std::runtime_error("ANM command size"); break;
        case 6: case 9: if (p.size() != 2U) throw std::runtime_error("ANM SFX size"); break;
        case 8: if (p.size() != kViewportBytes) throw std::runtime_error("ANM planar size"); break;
        default: throw std::runtime_error("ANM chunk id outside CODE.2 dispatcher");
    }
}
} // namespace

Cine00File::Cine00File(std::vector<std::uint8_t> source)
    : source_(std::move(source)) { index_and_validate(); }
const CineStats& Cine00File::stats() const noexcept { return stats_; }
const std::vector<std::uint8_t>& Cine00File::bytes() const noexcept {
    return source_;
}
std::vector<std::uint8_t> Cine00File::encode() const { return source_; }
std::size_t Cine00File::chunk_count(std::size_t frame) const {
    return frames_.at(frame).size();
}
CineChunkInfo Cine00File::info(std::size_t frame, std::size_t chunk) const {
    return frames_.at(frame).at(chunk);
}
std::vector<std::uint8_t> Cine00File::payload(
    std::size_t frame, std::size_t chunk) const {
    const auto c = info(frame, chunk);
    return {source_.begin() + static_cast<std::ptrdiff_t>(c.payload_offset),
            source_.begin() + static_cast<std::ptrdiff_t>(c.payload_offset + c.payload_bytes)};
}
void Cine00File::index_and_validate() {
    frames_.clear(); stats_ = {};
    std::size_t pos = 0U;
    stats_.frames = u16(source_, pos);
    stats_.default_wait = u16(source_, pos);
    frames_.reserve(stats_.frames);
    for (std::size_t i = 0U; i < stats_.frames; ++i) {
        const auto count = u16(source_, pos);
        auto& frame = frames_.emplace_back();
        frame.reserve(count);
        for (std::size_t j = 0U; j < count; ++j) {
            const auto id = u16(source_, pos);
            const auto declared = u32(source_, pos);
            if (id >= stats_.by_id.size() || (declared & 0x80000000U))
                throw std::runtime_error("ANM invalid chunk ID or negative size");
            const auto length = static_cast<std::size_t>(declared);
            if (pos > source_.size() || length > source_.size() - pos)
                throw std::runtime_error("ANM chunk exceeds file");
            frame.push_back({id, pos, length});
            const std::vector<std::uint8_t> data(
                source_.begin() + static_cast<std::ptrdiff_t>(pos),
                source_.begin() + static_cast<std::ptrdiff_t>(pos + length));
            validate_payload(id, data);
            pos += length;
            ++stats_.by_id[id]; ++stats_.chunks;
        }
    }
    if (pos != source_.size()) throw std::runtime_error("ANM trailing bytes");
}
std::vector<std::uint8_t> Cine00File::full_frame(
    std::size_t frame, std::size_t chunk) const {
    if (info(frame,chunk).id != 2U)
        throw std::invalid_argument("ANM chunk is not type-2 RLE");
    return decode_rle(payload(frame,chunk));
}
std::vector<std::uint8_t> Cine00File::viewport(
    std::size_t frame, std::size_t chunk) const {
    if (info(frame,chunk).id != 8U)
        throw std::invalid_argument("ANM chunk is not type-8 planar");
    const auto packed = payload(frame,chunk);
    std::vector<std::uint8_t> result(kViewportBytes);
    for (std::size_t y = 0U; y < kViewportHeight; ++y)
        for (std::size_t x = 0U; x < kWidth; ++x)
            result[y * kWidth + x] =
                packed[(x & 3U) * kPlaneBytes + y * 80U + (x >> 2U)];
    return result;
}
std::vector<std::uint8_t> Cine00File::encode_viewport(
    const std::vector<std::uint8_t>& pixels) {
    if (pixels.size() != kViewportBytes)
        throw std::invalid_argument("ANM viewport is not 320x256 indices");
    std::vector<std::uint8_t> packed(kViewportBytes);
    for (std::size_t y = 0U; y < kViewportHeight; ++y)
        for (std::size_t x = 0U; x < kWidth; ++x)
            packed[(x & 3U) * kPlaneBytes + y * 80U + (x >> 2U)] =
                pixels[y * kWidth + x];
    return packed;
}
void Cine00File::apply(const CineEdits& edits) {
    if (edits.expected_wait.has_value() != edits.replacement_wait.has_value())
        throw std::invalid_argument("ANM wait edit needs expected/replacement pair");
    if (edits.expected_wait && *edits.expected_wait != stats_.default_wait)
        throw std::runtime_error("ANM stale default wait");
    if (edits.chunks.empty() && !edits.expected_wait) return;
    using Key = std::pair<std::size_t,std::size_t>;
    std::map<Key,const CineChunkPatch*> patches;
    for (const auto& p : edits.chunks) {
        const auto c = info(p.frame,p.chunk);
        if (c.id == 4U)
            throw std::invalid_argument("ANM type-4 control editing is not certified");
        if (!patches.emplace(Key{p.frame,p.chunk},&p).second)
            throw std::invalid_argument("ANM duplicate chunk edit");
        if (p.expected_payload != payload(p.frame,p.chunk))
            throw std::runtime_error("ANM stale chunk payload");
        validate_payload(c.id, p.replacement_payload);
    }
    std::vector<std::uint8_t> rebuilt;
    rebuilt.reserve(source_.size());
    append16(rebuilt,static_cast<std::uint16_t>(stats_.frames));
    append16(rebuilt,edits.replacement_wait.value_or(stats_.default_wait));
    for (std::size_t i = 0U; i < frames_.size(); ++i) {
        append16(rebuilt,static_cast<std::uint16_t>(frames_[i].size()));
        for (std::size_t j = 0U; j < frames_[i].size(); ++j) {
            const auto c = frames_[i][j];
            const auto p = patches.find({i,j});
            const auto length = p == patches.end() ? c.payload_bytes
                                                   : p->second->replacement_payload.size();
            if (length > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()))
                throw std::overflow_error("ANM chunk exceeds signed 32-bit size");
            append16(rebuilt,c.id); append32(rebuilt,static_cast<std::uint32_t>(length));
            if (p == patches.end()) {
                rebuilt.insert(rebuilt.end(),
                    source_.begin() + static_cast<std::ptrdiff_t>(c.payload_offset),
                    source_.begin() + static_cast<std::ptrdiff_t>(c.payload_offset + length));
            } else {
                const auto& data = p->second->replacement_payload;
                rebuilt.insert(rebuilt.end(),data.begin(),data.end());
            }
        }
    }
    Cine00File candidate(std::move(rebuilt));
    *this = std::move(candidate);
}

} // namespace rloft
