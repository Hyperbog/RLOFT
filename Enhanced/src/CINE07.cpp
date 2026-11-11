#include "rloft/CINE07.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

std::uint16_t u16(const std::vector<std::uint8_t>& b, std::size_t& p) {
    if (p > b.size() || b.size() - p < 2U)
        throw std::runtime_error("CINE07 truncated frame word");
    const auto value = static_cast<std::uint16_t>(
        b[p] | (static_cast<unsigned>(b[p + 1U]) << 8U));
    p += 2U;
    return value;
}
std::uint32_t u32(const std::vector<std::uint8_t>& b, std::size_t& p) {
    if (p > b.size() || b.size() - p < 4U)
        throw std::runtime_error("CINE07 truncated packet size");
    const auto value = static_cast<std::uint32_t>(b[p]) |
        (static_cast<std::uint32_t>(b[p + 1U]) << 8U) |
        (static_cast<std::uint32_t>(b[p + 2U]) << 16U) |
        (static_cast<std::uint32_t>(b[p + 3U]) << 24U);
    p += 4U;
    return value;
}
void add16(std::vector<std::uint8_t>& b, std::uint16_t v) {
    b.push_back(static_cast<std::uint8_t>(v));
    b.push_back(static_cast<std::uint8_t>(v >> 8U));
}
void add32(std::vector<std::uint8_t>& b, std::uint32_t v) {
    for (unsigned shift = 0; shift < 32U; shift += 8U)
        b.push_back(static_cast<std::uint8_t>(v >> shift));
}
void append_frame(std::vector<std::uint8_t>& b, const Cine06Frame& f) {
    if (f.packets.size() > std::numeric_limits<std::uint16_t>::max())
        throw std::overflow_error("CINE07 replacement frame exceeds chunk limit");
    add16(b, static_cast<std::uint16_t>(f.packets.size()));
    for (const auto& p : f.packets) {
        if (p.id > 9U || p.id == 4U)
            throw std::invalid_argument("CINE07 unsupported replacement chunk ID");
        if (p.payload.size() > static_cast<std::size_t>(
                std::numeric_limits<std::int32_t>::max()))
            throw std::overflow_error("CINE07 replacement packet too large");
        add16(b, p.id);
        add32(b, static_cast<std::uint32_t>(p.payload.size()));
        b.insert(b.end(), p.payload.begin(), p.payload.end());
    }
}
} // namespace

Cine07File::Cine07File(std::vector<std::uint8_t> source,
                       std::uint16_t alternate_image_wait)
    : source_(std::move(source)), image_wait_(alternate_image_wait) {
    index_source();
}
const CineStats& Cine07File::stats() const noexcept { return source_.stats(); }
const std::vector<std::uint8_t>& Cine07File::bytes() const noexcept {
    return source_.bytes();
}
std::vector<std::uint8_t> Cine07File::encode() const { return source_.encode(); }
const std::vector<Cine07FrameIndex>& Cine07File::timeline() const noexcept {
    return timeline_;
}
std::uint64_t Cine07File::total_ticks() const noexcept { return total_ticks_; }
const Cine07FrameIndex& Cine07File::frame_index(std::size_t f) const {
    return timeline_.at(f);
}
Cine06Frame Cine07File::frame(std::size_t f) const { return source_.frame(f); }
Cine02VisualState Cine07File::snapshot(std::size_t f) const {
    return source_.snapshot(f);
}
std::vector<Cine01Cue> Cine07File::cues() const { return source_.cues(); }

void Cine07File::index_source() {
    const auto& data = bytes();
    std::size_t pos = 0U;
    const auto count = u16(data, pos);
    const auto wait = u16(data, pos);
    if (count != stats().frames || wait != stats().default_wait)
        throw std::logic_error("CINE07 source index differs from canonical codec");
    std::vector<Cine07FrameIndex> indexed;
    indexed.reserve(count);
    std::optional<std::size_t> last_palette, last_image, subtitle;
    std::uint64_t tick = 0U;
    for (std::size_t f = 0U; f < count; ++f) {
        const auto start = pos;
        const auto chunks = u16(data, pos);
        bool alternate = false;
        for (std::size_t c = 0U; c < chunks; ++c) {
            const auto id = u16(data, pos);
            const auto size = u32(data, pos);
            if (id > 9U || (size & 0x80000000U) != 0U ||
                pos > data.size() || size > data.size() - pos)
                throw std::logic_error("CINE07 index encountered invalid canonical packet");
            if (id == 0U || id == 1U) last_palette = f;
            if (id == 1U || id == 2U || id == 8U ||
                (id == 3U && size > 2U)) last_image = f;
            if (id == 3U || id == 8U) alternate = true;
            if (id == 7U) {
                if (size != 4U) throw std::logic_error("CINE07 invalid subtitle packet");
                std::size_t word_at = pos;
                subtitle = u16(data, word_at) == 0U
                    ? std::nullopt : std::optional<std::size_t>(f);
            }
            pos += size;
        }
        const std::uint16_t duration = alternate ? image_wait_ : wait;
        indexed.push_back({f, start, pos, tick, duration,
                           last_palette, last_image, subtitle});
        tick += duration;
    }
    if (pos != data.size())
        throw std::logic_error("CINE07 source index leaves trailing bytes");
    timeline_ = std::move(indexed);
    total_ticks_ = tick;
}

Cine07Seek Cine07File::seek_tick(std::uint64_t tick) const {
    if (tick >= total_ticks_)
        throw std::out_of_range("CINE07 tick is outside cinematic duration");
    const auto it = std::upper_bound(
        timeline_.begin(), timeline_.end(), tick,
        [](std::uint64_t t, const Cine07FrameIndex& f) {
            return t < f.start_tick;
        });
    if (it == timeline_.begin())
        throw std::logic_error("CINE07 timing index has no initial frame");
    const auto& f = *(it - 1);
    if (tick - f.start_tick >= f.duration_ticks)
        throw std::logic_error("CINE07 timing index contains a gap");
    return {f, static_cast<std::uint16_t>(tick - f.start_tick)};
}

std::vector<std::uint8_t> Cine07File::frame_bytes(
    std::size_t first, std::size_t last) const {
    if (first > last || last > timeline_.size())
        throw std::out_of_range("CINE07 frame byte range");
    const auto begin = first == timeline_.size()
        ? bytes().size() : timeline_[first].byte_begin;
    const auto end = last == timeline_.size()
        ? bytes().size() : timeline_[last].byte_begin;
    return {bytes().begin() + static_cast<std::ptrdiff_t>(begin),
            bytes().begin() + static_cast<std::ptrdiff_t>(end)};
}
std::vector<std::uint8_t> Cine07File::extract_range(
    std::size_t first, std::size_t last) const {
    if (first >= last || last > timeline_.size())
        throw std::out_of_range("CINE07 extract requires a nonempty frame range");
    auto result = std::vector<std::uint8_t>{};
    const auto first_byte = timeline_[first].byte_begin;
    const auto end_byte = last == timeline_.size()
        ? bytes().size() : timeline_[last].byte_begin;
    result.reserve(4U + end_byte - first_byte);
    add16(result, static_cast<std::uint16_t>(last - first));
    add16(result, stats().default_wait);
    result.insert(result.end(),
                  bytes().begin() + static_cast<std::ptrdiff_t>(first_byte),
                  bytes().begin() + static_cast<std::ptrdiff_t>(end_byte));
    (void)Cine06File(result); // Structural validity; no state priming implied.
    return result;
}

void Cine07File::apply(const Cine07Edits& edits) {
    if (edits.expected_wait.has_value() != edits.replacement_wait.has_value())
        throw std::invalid_argument("CINE07 wait change requires both values");
    if (edits.ranges.empty() && !edits.expected_wait) return;
    if (edits.expected_wait && *edits.expected_wait != stats().default_wait)
        throw std::runtime_error("CINE07 stale default wait");
    if (edits.expected_frame_count &&
        *edits.expected_frame_count != stats().frames)
        throw std::runtime_error("CINE07 stale frame count");
    if (!edits.ranges.empty() && !edits.expected_frame_count)
        throw std::invalid_argument("CINE07 range edits require expected frame count");

    std::vector<const Cine07RangeEdit*> sorted;
    sorted.reserve(edits.ranges.size());
    for (const auto& change : edits.ranges) sorted.push_back(&change);
    std::sort(sorted.begin(), sorted.end(), [](const auto* a, const auto* b) {
        if (a->first != b->first) return a->first < b->first;
        return a->last < b->last;
    });
    std::size_t removed_total = 0U;
    std::size_t inserted_total = 0U;
    std::size_t cursor = 0U;
    std::optional<std::size_t> previous_start;
    for (const auto* edit : sorted) {
        if (edit->first > edit->last || edit->last > stats().frames)
            throw std::out_of_range("CINE07 edit frame range");
        if (edit->first < cursor ||
            (previous_start && edit->first == *previous_start))
            throw std::invalid_argument("CINE07 overlapping or duplicate ranges");
        if (frame_bytes(edit->first, edit->last) != edit->expected_frame_bytes)
            throw std::runtime_error("CINE07 stale original frame bytes");
        removed_total += edit->last - edit->first;
        if (edit->replacements.size() >
            std::numeric_limits<std::uint16_t>::max() - inserted_total)
            throw std::overflow_error("CINE07 insertion total exceeds uint16");
        inserted_total += edit->replacements.size();
        previous_start = edit->first;
        cursor = edit->last;
    }
    const auto remaining = stats().frames - removed_total;
    if (inserted_total > std::numeric_limits<std::uint16_t>::max() - remaining)
        throw std::overflow_error("CINE07 frame count exceeds uint16");
    const auto result_count = remaining + inserted_total;

    std::vector<std::uint8_t> rebuilt;
    rebuilt.reserve(bytes().size());
    add16(rebuilt, static_cast<std::uint16_t>(result_count));
    add16(rebuilt, edits.replacement_wait.value_or(stats().default_wait));
    cursor = 0U;
    for (const auto* edit : sorted) {
        const auto original = frame_bytes(cursor, edit->first);
        rebuilt.insert(rebuilt.end(), original.begin(), original.end());
        for (const auto& replacement : edit->replacements)
            append_frame(rebuilt, replacement);
        cursor = edit->last;
    }
    const auto tail = frame_bytes(cursor, stats().frames);
    rebuilt.insert(rebuilt.end(), tail.begin(), tail.end());
    Cine07File verified(std::move(rebuilt), image_wait_);
    *this = std::move(verified);
}

} // namespace rloft
