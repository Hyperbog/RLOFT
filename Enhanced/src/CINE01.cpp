#include "rloft/CINE01.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

bool is_cue_id(std::uint16_t id) noexcept {
    return id == 5U || id == 6U || id == 7U || id == 9U;
}

bool suppressed_queued_sound(std::uint16_t encoded) noexcept {
    return encoded == 3U || encoded == 15U || encoded == 0x88U;
}

std::uint16_t word(const std::vector<std::uint8_t>& b, std::size_t pos) {
    if (pos > b.size() || b.size() - pos < 2U)
        throw std::runtime_error("CINE01 command parameter truncated");
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[pos]) |
        (static_cast<unsigned>(b[pos + 1U]) << 8U));
}

void append_word(std::vector<std::uint8_t>& b, std::uint16_t value) {
    b.push_back(static_cast<std::uint8_t>(value));
    b.push_back(static_cast<std::uint8_t>(value >> 8U));
}

} // namespace

Cine01File::Cine01File(std::vector<std::uint8_t> source)
    : direct_(std::move(source)) {}

const CineStats& Cine01File::stats() const noexcept { return direct_.stats(); }
const std::vector<std::uint8_t>& Cine01File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Cine01File::encode() const {
    return direct_.encode();
}
std::size_t Cine01File::chunk_count(std::size_t frame) const {
    return direct_.chunk_count(frame);
}
CineChunkInfo Cine01File::info(std::size_t frame, std::size_t chunk) const {
    return direct_.info(frame, chunk);
}
std::vector<std::uint8_t> Cine01File::payload(
    std::size_t frame, std::size_t chunk) const {
    return direct_.payload(frame, chunk);
}
std::vector<std::uint8_t> Cine01File::full_frame(
    std::size_t frame, std::size_t chunk) const {
    return direct_.full_frame(frame, chunk);
}
std::vector<std::uint8_t> Cine01File::viewport(
    std::size_t frame, std::size_t chunk) const {
    return direct_.viewport(frame, chunk);
}

Cine01FrameInfo Cine01File::frame_info(std::size_t frame) const {
    const auto count = chunk_count(frame);
    bool alternate = false;
    for (std::size_t i = 0U; i < count; ++i) {
        const auto id = info(frame, i).id;
        if (id == 3U || id == 8U) alternate = true;
    }
    return {frame, count, alternate,
            static_cast<std::uint16_t>(alternate ? 6U : stats().default_wait)};
}

Cine01Cue Cine01File::cue(std::size_t frame, std::size_t chunk) const {
    const auto id = info(frame, chunk).id;
    if (!is_cue_id(id))
        throw std::invalid_argument("CINE01 chunk is not an audiovisual command");
    const auto data = payload(frame, chunk);
    const auto first = word(data, 0U);
    const std::uint16_t second = (id == 5U || id == 7U)
        ? word(data, 2U) : std::uint16_t{0U};
    return {frame, chunk, static_cast<Cine01CueKind>(id), first, second,
            id == 6U && suppressed_queued_sound(first)};
}

std::vector<Cine01Cue> Cine01File::cues() const {
    std::vector<Cine01Cue> out;
    for (std::size_t f = 0U; f < stats().frames; ++f) {
        for (std::size_t c = 0U; c < chunk_count(f); ++c) {
            if (is_cue_id(info(f, c).id)) out.push_back(cue(f, c));
        }
    }
    return out;
}
std::vector<Cine01Cue> Cine01File::cues_at(std::size_t frame) const {
    const auto count = chunk_count(frame);
    std::vector<Cine01Cue> out;
    for (std::size_t c = 0U; c < count; ++c)
        if (is_cue_id(info(frame, c).id)) out.push_back(cue(frame, c));
    return out;
}

void Cine01File::apply(const Cine01Edits& edits) {
    CineEdits merged = edits.packets;
    merged.chunks.reserve(merged.chunks.size() + edits.cues.size());
    for (const auto& patch : edits.cues) {
        const auto old = cue(patch.frame, patch.chunk);
        if (old.kind != patch.expected_kind ||
            old.first != patch.expected_first ||
            old.second != patch.expected_second)
            throw std::runtime_error("CINE01 stale or incorrect command expectation");
        const auto id = static_cast<std::uint16_t>(old.kind);
        if ((id == 6U || id == 9U) && patch.replacement_second != 0U)
            throw std::invalid_argument("CINE01 two-byte command has no second word");
        std::vector<std::uint8_t> replacement;
        replacement.reserve(id == 5U || id == 7U ? 4U : 2U);
        append_word(replacement, patch.replacement_first);
        if (id == 5U || id == 7U)
            append_word(replacement, patch.replacement_second);
        merged.chunks.push_back({patch.frame, patch.chunk,
                                 payload(patch.frame, patch.chunk),
                                 std::move(replacement)});
    }
    Cine00File candidate(encode());
    candidate.apply(merged);
    direct_ = std::move(candidate);
}

} // namespace rloft
