#include "rloft/CINE06.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

void add16(std::vector<std::uint8_t>& out, std::uint16_t value) {
    out.push_back(static_cast<std::uint8_t>(value));
    out.push_back(static_cast<std::uint8_t>(value >> 8U));
}

void add32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    for (unsigned shift = 0U; shift < 32U; shift += 8U)
        out.push_back(static_cast<std::uint8_t>(value >> shift));
}

bool identical(const Cine06Frame& a, const Cine06Frame& b) {
    if (a.packets.size() != b.packets.size()) return false;
    for (std::size_t i = 0U; i < a.packets.size(); ++i)
        if (a.packets[i].id != b.packets[i].id ||
            a.packets[i].payload != b.packets[i].payload) return false;
    return true;
}

void write_frame(std::vector<std::uint8_t>& out, const Cine06Frame& frame) {
    if (frame.packets.size() > std::numeric_limits<std::uint16_t>::max())
        throw std::overflow_error("CINE06 frame exceeds 16-bit chunk count");
    add16(out, static_cast<std::uint16_t>(frame.packets.size()));
    for (const auto& packet : frame.packets) {
        if (packet.id > 9U || packet.id == 4U)
            throw std::invalid_argument("CINE06 unsupported new control packet");
        if (packet.payload.size() >
            static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()))
            throw std::overflow_error("CINE06 packet exceeds signed 32-bit length");
        add16(out, packet.id);
        add32(out, static_cast<std::uint32_t>(packet.payload.size()));
        out.insert(out.end(), packet.payload.begin(), packet.payload.end());
    }
}

} // namespace

Cine06File::Cine06File(std::vector<std::uint8_t> source)
    : source_(std::move(source)) {}

const CineStats& Cine06File::stats() const noexcept { return source_.stats(); }
const std::vector<std::uint8_t>& Cine06File::bytes() const noexcept {
    return source_.bytes();
}
std::vector<std::uint8_t> Cine06File::encode() const {
    return source_.encode();
}
Cine06Frame Cine06File::frame(std::size_t index) const {
    const auto count = source_.frame_info(index).chunks;
    Cine06Frame result;
    result.packets.reserve(count);
    for (std::size_t i = 0U; i < count; ++i)
        result.packets.push_back({source_.info(index, i).id,
                                  source_.payload(index, i)});
    return result;
}
Cine01FrameInfo Cine06File::frame_info(std::size_t index) const {
    return source_.frame_info(index);
}
std::vector<Cine01Cue> Cine06File::cues() const {
    return source_.cues();
}
Cine02VisualState Cine06File::snapshot(std::size_t index) const {
    return source_.snapshot(index);
}

void Cine06File::apply(const Cine06Edits& edits) {
    const bool frame_edits = !edits.changes.empty() || !edits.inserts.empty();
    const bool wait_edit = edits.expected_wait || edits.replacement_wait;
    if (!frame_edits && !wait_edit) return;
    if (edits.expected_wait.has_value() !=
        edits.replacement_wait.has_value())
        throw std::invalid_argument("CINE06 wait edit requires both words");
    if (edits.expected_wait && *edits.expected_wait != stats().default_wait)
        throw std::runtime_error("CINE06 stale default wait");
    if (frame_edits && (!edits.expected_frames ||
                        *edits.expected_frames != stats().frames))
        throw std::runtime_error("CINE06 stale or missing original frame count");
    if (edits.expected_frames && *edits.expected_frames != stats().frames)
        throw std::runtime_error("CINE06 frame count changed since request");

    const auto count = stats().frames;
    std::map<std::size_t, const Cine06FrameChange*> changes;
    std::map<std::size_t, const Cine06FrameInsert*> inserts;
    for (const auto& edit : edits.changes) {
        if (edit.index >= count)
            throw std::out_of_range("CINE06 replacement frame index");
        if (!changes.emplace(edit.index, &edit).second)
            throw std::invalid_argument("CINE06 duplicate original frame edit");
        if (!identical(frame(edit.index), edit.expected))
            throw std::runtime_error("CINE06 stale original frame contents");
    }
    for (const auto& edit : edits.inserts) {
        if (edit.before > count)
            throw std::out_of_range("CINE06 insertion anchor frame index");
        if (!inserts.emplace(edit.before, &edit).second)
            throw std::invalid_argument("CINE06 duplicate frame insert anchor");
        if (edit.before == count) {
            if (edit.expected_next)
                throw std::invalid_argument("CINE06 append has no anchor frame");
        } else if (!edit.expected_next ||
                   !identical(frame(edit.before), *edit.expected_next)) {
            throw std::runtime_error("CINE06 stale insertion anchor frame");
        }
    }
    const std::size_t removed = [&] {
        std::size_t n = 0U;
        for (const auto& entry : changes)
            if (!entry.second->replacement) ++n;
        return n;
    }();
    if (inserts.size() > std::numeric_limits<std::uint16_t>::max() - count + removed)
        throw std::overflow_error("CINE06 ANM frame count exceeds 16 bits");
    const auto result_count = count - removed + inserts.size();

    std::vector<std::uint8_t> rebuilt;
    rebuilt.reserve(bytes().size());
    add16(rebuilt, static_cast<std::uint16_t>(result_count));
    add16(rebuilt, edits.replacement_wait.value_or(stats().default_wait));
    for (std::size_t i = 0U; i <= count; ++i) {
        const auto insertion = inserts.find(i);
        if (insertion != inserts.end()) write_frame(rebuilt, insertion->second->inserted);
        if (i == count) break;
        const auto change = changes.find(i);
        if (change == changes.end()) {
            // Verbatim source frame bytes; the shared codec validated them.
            const auto original = frame(i);
            // Preserve unobserved type-4 packets in untouched frames.
            add16(rebuilt, static_cast<std::uint16_t>(original.packets.size()));
            for (const auto& packet : original.packets) {
                add16(rebuilt, packet.id);
                add32(rebuilt, static_cast<std::uint32_t>(packet.payload.size()));
                rebuilt.insert(rebuilt.end(), packet.payload.begin(), packet.payload.end());
            }
        } else if (change->second->replacement) {
            write_frame(rebuilt, *change->second->replacement);
        }
    }
    // Parse every packet with the existing original-executable-backed codec,
    // including exact RLE scanlines, delta bounds, and planar image geometry.
    Cine02File candidate(std::move(rebuilt));
    source_ = std::move(candidate); // Single commit only after full validation.
}

} // namespace rloft
