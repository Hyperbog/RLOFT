#include "rloft/LEVELS.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kPreamble = 0x1DU;
constexpr std::size_t kHeader = 0x2A4U;
constexpr std::size_t kEvents = 0x514U;
constexpr std::size_t kEventSlots = 50U;
constexpr std::size_t kEventStride = 26U;

void available(std::size_t pos, std::size_t size, std::size_t end) {
    if (pos > end || size > end - pos)
        throw std::runtime_error("LEVELS.DAT truncated or corrupt section");
}
std::uint16_t le16(const std::vector<std::uint8_t>& b, std::size_t p) {
    available(p, 2U, b.size());
    return static_cast<std::uint16_t>(b[p] | (unsigned(b[p + 1U]) << 8U));
}
std::int32_t signed16(const std::vector<std::uint8_t>& b, std::size_t p) {
    const auto n = le16(b, p);
    return n <= 32767U ? static_cast<std::int32_t>(n)
                       : static_cast<std::int32_t>(n) - 65536;
}
std::uint32_t le32(const std::vector<std::uint8_t>& b, std::size_t p) {
    available(p, 4U, b.size());
    return std::uint32_t(b[p]) |
        (std::uint32_t(b[p + 1U]) << 8U) |
        (std::uint32_t(b[p + 2U]) << 16U) |
        (std::uint32_t(b[p + 3U]) << 24U);
}
void put16(std::vector<std::uint8_t>& b, std::size_t p, std::uint16_t n) {
    b.at(p) = static_cast<std::uint8_t>(n);
    b.at(p + 1U) = static_cast<std::uint8_t>(n >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t p, std::uint32_t n) {
    for (unsigned i = 0; i < 4U; ++i)
        b.at(p + i) = static_cast<std::uint8_t>(n >> (8U * i));
}
}  // namespace

LevelsRecordSummary LevelsArchive::validate_record(
    const std::vector<std::uint8_t>& b) {
    available(0U, kPreamble + kHeader, b.size());
    std::uint32_t bounds[7]{};
    for (std::size_t i = 0; i < 7U; ++i)
        bounds[i] = le32(b, 1U + 4U * i);
    if (bounds[0] != kPreamble || bounds[6] > b.size() ||
        !std::is_sorted(std::begin(bounds), std::end(bounds)))
        throw std::runtime_error("LEVELS.DAT invalid compact boundaries");

    const auto width = signed16(b, kPreamble + 1U);
    const auto height = signed16(b, kPreamble + 3U);
    if (width <= 0 || height <= 0)
        throw std::runtime_error("LEVELS.DAT invalid width or height");
    const auto cells = static_cast<std::size_t>(width) *
                       static_cast<std::size_t>(height);
    if (cells > (b.size() - kPreamble - kHeader) / 2U ||
        bounds[1] != kPreamble + kHeader + cells * 2U)
        throw std::runtime_error("LEVELS.DAT grid boundary mismatch");

    LevelsRecordSummary out{};
    out.width = static_cast<int>(width);
    out.height = static_cast<int>(height);
    out.cell_count = cells;
    out.environment_profile = b[kPreamble + 0x290U];
    out.music_sequence = b[kPreamble + 0x29CU];

    std::size_t cursor = bounds[1];
    for (int column = 0; column < width; ++column) {
        available(cursor, 2U, bounds[2]);
        const auto count = signed16(b, cursor);
        cursor += 2U;
        if (count < 0 || static_cast<std::size_t>(count) >
                             (bounds[2] - cursor) / 10U)
            throw std::runtime_error("LEVELS.DAT invalid 10-byte control count");
        for (int i = 0; i < count; ++i) {
            if (b[cursor + 5U] != static_cast<std::uint8_t>(column) ||
                b[cursor + 4U] >= height)
                throw std::runtime_error("LEVELS.DAT control coordinate mismatch");
            cursor += 10U;
        }
        out.control_records += static_cast<std::size_t>(count);
    }
    if (cursor != bounds[2])
        throw std::runtime_error("LEVELS.DAT control-list boundary mismatch");

    const auto a = signed16(b, kPreamble + 0x28AU);
    const auto c = signed16(b, kPreamble + 0x28CU);
    const auto placed = signed16(b, kPreamble + 0x28EU);
    if (a < 0 || c < 0 || placed < 0 ||
        bounds[3] - bounds[2] != static_cast<std::size_t>(a) * 11U ||
        bounds[4] - bounds[3] != static_cast<std::size_t>(placed) * 11U ||
        bounds[5] - bounds[4] != static_cast<std::size_t>(c) * 11U ||
        bounds[6] - bounds[5] != kEvents)
        throw std::runtime_error("LEVELS.DAT compact world/event boundary mismatch");
    out.family_a_npcs = static_cast<std::size_t>(a);
    out.family_b_objects = static_cast<std::size_t>(placed);
    out.family_c_loose_items = static_cast<std::size_t>(c);

    for (std::size_t i = 0; i < kEventSlots; ++i) {
        const auto active = b[static_cast<std::size_t>(bounds[5]) +
                              i * kEventStride];
        if (active > 5U)
            throw std::runtime_error("LEVELS.DAT event slot has too many commands");
        if (active != 0U) ++out.active_event_slots;
        out.active_event_commands += active;
    }

    cursor = bounds[6];
    available(cursor, 4U + cells + 4U, b.size());
    if (signed16(b, cursor) != width || signed16(b, cursor + 2U) != height)
        throw std::runtime_error("LEVELS.DAT automap dimension mismatch");
    cursor += 4U + cells;
    const auto annotations = le32(b, cursor);
    cursor += 4U;
    if (annotations != b.size() - cursor)
        throw std::runtime_error("LEVELS.DAT annotation length mismatch");
    out.annotation_bytes = annotations;
    return out;
}

LevelsArchive::LevelsArchive(const std::vector<std::uint8_t>& bytes) {
    available(0U, 4U, bytes.size());
    const auto count = static_cast<std::size_t>(le32(bytes, 0U));
    if (count == 0U || count > (bytes.size() - 4U) / 4U ||
        count > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::runtime_error("LEVELS.DAT invalid record count");
    const std::size_t directory_end = 4U + 4U * count;
    if (le32(bytes, 4U) != directory_end)
        throw std::runtime_error("LEVELS.DAT noncanonical first offset");
    records_.reserve(count);
    summaries_.reserve(count);
    std::size_t begin = directory_end;
    for (std::size_t i = 0; i < count; ++i) {
        const auto at = static_cast<std::size_t>(le32(bytes, 4U + i * 4U));
        const auto end = i + 1U < count
            ? static_cast<std::size_t>(le32(bytes, 4U + (i + 1U) * 4U))
            : bytes.size();
        if (at != begin || end <= at || end > bytes.size())
            throw std::runtime_error("LEVELS.DAT invalid record offsets");
        std::vector<std::uint8_t> rec(
            bytes.begin() + static_cast<std::ptrdiff_t>(at),
            bytes.begin() + static_cast<std::ptrdiff_t>(end));
        summaries_.push_back(validate_record(rec));
        records_.push_back(std::move(rec));
        begin = end;
    }
}

std::size_t LevelsArchive::level_count() const noexcept { return records_.size(); }

std::vector<std::uint8_t> LevelsArchive::record(std::size_t id) const {
    return records_.at(id);
}

const LevelsRecordSummary& LevelsArchive::summary(std::size_t id) const {
    return summaries_.at(id);
}

std::vector<std::uint8_t> LevelsArchive::encode() const {
    const auto limit = static_cast<std::uint64_t>(
        std::numeric_limits<std::uint32_t>::max());
    std::uint64_t size = 4U + records_.size() * 4ULL;
    for (const auto& rec : records_) {
        if (rec.size() > limit - size)
            throw std::overflow_error("LEVELS.DAT exceeds 32-bit offsets");
        size += rec.size();
    }
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size), 0U);
    put32(bytes, 0U, static_cast<std::uint32_t>(records_.size()));
    std::size_t at = 4U + 4U * records_.size();
    for (std::size_t i = 0; i < records_.size(); ++i) {
        put32(bytes, 4U + i * 4U, static_cast<std::uint32_t>(at));
        std::copy(records_[i].begin(), records_[i].end(),
                  bytes.begin() + static_cast<std::ptrdiff_t>(at));
        at += records_[i].size();
    }
    return bytes;
}

void LevelsArchive::replace_record(
    std::size_t id, const std::vector<std::uint8_t>& rec) {
    if (id >= records_.size()) throw std::out_of_range("LEVELS.DAT level id");
    const auto checked = validate_record(rec);
    const auto limit = static_cast<std::uint64_t>(
        std::numeric_limits<std::uint32_t>::max());
    std::uint64_t size = 4U + records_.size() * 4ULL;
    for (std::size_t i = 0; i < records_.size(); ++i) {
        const auto& r = i == id ? rec : records_[i];
        if (r.size() > limit - size)
            throw std::overflow_error("LEVELS.DAT replacement exceeds 32-bit offsets");
        size += r.size();
    }
    auto replacement = rec;
    records_[id].swap(replacement);
    summaries_[id] = checked;
}

void LevelsArchive::set_cell_word(
    std::size_t id, int column, int row, std::uint16_t word) {
    const auto& shape = summary(id);
    if (column < 0 || row < 0 || column >= shape.width || row >= shape.height)
        throw std::out_of_range("LEVELS.DAT cell coordinate");
    auto rec = record(id);
    const std::size_t position = kPreamble + kHeader + 2U * (
        static_cast<std::size_t>(row) * static_cast<std::size_t>(shape.width) +
        static_cast<std::size_t>(column));
    put16(rec, position, word);
    replace_record(id, rec);
}

void LevelsArchive::set_music_sequence(std::size_t id, std::uint8_t res5_id) {
    auto rec = record(id);
    rec[kPreamble + 0x29CU] = res5_id;
    replace_record(id, rec);
}

}  // namespace rloft
