#include "rloft/LEV001.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kHeaderAt = 0x1DU;
constexpr std::size_t kHeaderSize = 0x2A4U;
constexpr std::size_t kGridAt = kHeaderAt + kHeaderSize;
constexpr std::size_t kEvents = 50U;
constexpr std::size_t kEventStride = 26U;
constexpr std::size_t kMaxFileSize = 4U * 1024U * 1024U;
constexpr std::size_t kMaxAnnotationBytes = 2046U;

std::uint16_t read16(const std::vector<std::uint8_t>& b, std::size_t at) {
    return static_cast<std::uint16_t>(b.at(at) |
            (static_cast<unsigned>(b.at(at + 1U)) << 8U));
}
void write16(std::vector<std::uint8_t>& b, std::size_t at, unsigned n) {
    b.at(at) = static_cast<std::uint8_t>(n);
    b.at(at + 1U) = static_cast<std::uint8_t>(n >> 8U);
}
void write32(std::vector<std::uint8_t>& b, std::size_t at, std::uint32_t n) {
    for (unsigned i = 0U; i < 4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(n >> (8U * i));
}
std::size_t family_index(LevWorldFamily family) {
    const auto index = static_cast<unsigned>(family);
    if (index > 2U) throw std::invalid_argument("invalid direct-level family");
    return index;
}
} // namespace

LevDirectSummary Lev001File::inspect(const std::vector<std::uint8_t>& b) {
    if (b.size() < kGridAt || b.size() > kMaxFileSize)
        throw std::runtime_error("direct-level file size invalid");
    // Authoritative CODE.2 direct-save grammar; no second world decoder.
    const auto level = ExplorationStateCodec::parse_direct_level(b);
    if (level.storage != LevelStorage::DirectSave ||
        level.width > 128 || level.height > 128)
        throw std::runtime_error("invalid direct-save level geometry");
    LevDirectSummary out{};
    out.width = level.width;
    out.height = level.height;
    out.music_sequence = level.music_sequence;
    out.writer_unowned_byte = level.writer_unowned_marker;
    out.active_animation_count = level.active_animation_count;
    out.boundaries = level.boundaries;
    out.group1_controls = level.group1.size();
    out.group2_controls = level.group2.size();
    out.world_counts = {{level.family_a_raw.size()/22U,
                         (level.family_b_raw.size()-2U)/22U,
                         level.family_c_raw.size()/22U}};
    out.annotation_bytes = level.automap_annotations.size();
    if (out.annotation_bytes > kMaxAnnotationBytes)
        throw std::runtime_error("direct-level notes exceed original 0x7FF buffer");
    for (std::size_t i = 0; i < kEvents; ++i) {
        const auto active = level.event_scripts[i*kEventStride];
        if (active > 5U) throw std::runtime_error("invalid LEV script command count");
        out.active_event_slots += active != 0U ? 1U : 0U;
        out.active_event_commands += active;
    }
    return out;
}

Lev001File::Lev001File(std::vector<std::uint8_t> original)
    : bytes_(std::move(original)), summary_(inspect(bytes_)) {}

LevelState Lev001File::decode() const {
    return ExplorationStateCodec::parse_direct_level(bytes_);
}

void Lev001File::accept(std::vector<std::uint8_t> candidate) {
    const auto checked = inspect(candidate); // No mutation on failure.
    bytes_.swap(candidate);
    summary_ = checked;
}

std::size_t Lev001File::grid_word_at(int column, int row) const {
    if (column < 0 || row < 0 || column >= summary_.width || row >= summary_.height)
        throw std::out_of_range("LEV cell coordinate outside original grid");
    return kGridAt + 2U * (static_cast<std::size_t>(row) *
        static_cast<std::size_t>(summary_.width) + static_cast<std::size_t>(column));
}

std::uint16_t Lev001File::cell_word(int column, int row) const {
    return read16(bytes_, grid_word_at(column, row));
}

void Lev001File::set_cell_word(int column, int row, std::uint16_t word) {
    const auto at = grid_word_at(column, row);
    auto candidate = bytes_;
    write16(candidate, at, word);
    accept(std::move(candidate));
}

void Lev001File::set_music_sequence(std::uint8_t res5_index) {
    auto candidate = bytes_;
    candidate[kHeaderAt + 0x29CU] = res5_index;
    accept(std::move(candidate));
}

void Lev001File::replace_world_family(
    LevWorldFamily family, const std::vector<std::uint8_t>& records22) {
    const auto family_id = family_index(family);
    if (records22.size() % 22U != 0U || records22.size()/22U > 32767U)
        throw std::invalid_argument("direct-level family requires 22-byte records");
    const auto count = records22.size()/22U;
    const auto start = static_cast<std::size_t>(summary_.boundaries[2U + family_id]);
    const auto end = static_cast<std::size_t>(summary_.boundaries[3U + family_id]);
    std::vector<std::uint8_t> block;
    block.reserve(records22.size() + (family_id == 1U ? 2U : 0U));
    if (family_id == 1U) {
        block.push_back(static_cast<std::uint8_t>(count));
        block.push_back(static_cast<std::uint8_t>(count >> 8U));
    }
    block.insert(block.end(), records22.begin(), records22.end());
    const auto new_size = bytes_.size() - (end - start) + block.size();
    if (new_size > kMaxFileSize)
        throw std::invalid_argument("direct-level replacement exceeds limit");
    const auto shift = static_cast<std::int64_t>(block.size()) -
                       static_cast<std::int64_t>(end - start);
    auto candidate = bytes_;
    candidate.erase(candidate.begin() + static_cast<std::ptrdiff_t>(start),
                    candidate.begin() + static_cast<std::ptrdiff_t>(end));
    candidate.insert(candidate.begin() + static_cast<std::ptrdiff_t>(start),
                     block.begin(), block.end());
    const std::size_t counter = family_id == 0U ? 0x28AU :
                                family_id == 1U ? 0x28EU : 0x28CU;
    write16(candidate, kHeaderAt + counter, static_cast<unsigned>(count));
    for (std::size_t i = 3U + family_id; i < 7U; ++i) {
        const auto shifted = static_cast<std::int64_t>(summary_.boundaries[i]) + shift;
        if (shifted < 0 || shifted > std::numeric_limits<std::uint32_t>::max())
            throw std::overflow_error("direct-level section boundary overflow");
        write32(candidate, 1U + i*4U, static_cast<std::uint32_t>(shifted));
    }
    accept(std::move(candidate));
}

void Lev001File::replace_automap_annotations(
    const std::vector<std::uint8_t>& blob) {
    if (blob.size() > kMaxAnnotationBytes)
        throw std::invalid_argument("automap annotations exceed original buffer");
    const auto cells = static_cast<std::size_t>(summary_.width) *
                       static_cast<std::size_t>(summary_.height);
    const auto header = static_cast<std::size_t>(summary_.boundaries[6]) + 4U + cells;
    auto candidate = bytes_;
    candidate.resize(header + 4U);
    write32(candidate, header, static_cast<std::uint32_t>(blob.size()));
    candidate.insert(candidate.end(), blob.begin(), blob.end());
    accept(std::move(candidate));
}

} // namespace rloft
