#include "rloft/exploration_state.hpp"

#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::size_t kPreambleSize = 0x1D;
constexpr std::size_t kHeaderSize = 0x2A4;
constexpr std::size_t kEventSize = 0x514;

std::uint16_t u16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::int16_t s16(const std::uint8_t* p) {
    return static_cast<std::int16_t>(u16(p));
}

std::uint32_t u32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

void require_range(std::size_t start, std::size_t size, std::size_t limit,
                   const char* what) {
    if (start > limit || size > limit - start) {
        throw std::runtime_error(std::string("truncated ") + what);
    }
}

std::vector<std::uint8_t> slice(const std::vector<std::uint8_t>& bytes,
                                std::size_t start, std::size_t end,
                                const char* what) {
    if (end < start) throw std::runtime_error(std::string("invalid ") + what);
    require_range(start, end - start, bytes.size(), what);
    return {bytes.begin() + static_cast<std::ptrdiff_t>(start),
            bytes.begin() + static_cast<std::ptrdiff_t>(end)};
}

void read_list_group(const std::vector<std::uint8_t>& record,
                     std::size_t& pos,
                     int width,
                     int height,
                     std::vector<CellControlRecord>& out,
                     bool split_legacy,
                     std::vector<CellControlRecord>* split_other) {
    for (int column = 0; column < width; ++column) {
        require_range(pos, 2, record.size(), "10-byte list count");
        const int count = s16(record.data() + pos);
        pos += 2;
        if (count < 0) throw std::runtime_error("negative 10-byte list count");
        for (int i = 0; i < count; ++i) {
            require_range(pos, 10, record.size(), "10-byte cell record");
            CellControlRecord item{};
            std::copy_n(record.begin() + static_cast<std::ptrdiff_t>(pos),
                        item.raw.size(), item.raw.begin());
            pos += item.raw.size();
            if (item.column() != column || item.row() >= height) {
                throw std::runtime_error("10-byte cell record coordinate mismatch");
            }
            if (split_legacy && item.kind() == 1) {
                split_other->push_back(item);
            } else {
                out.push_back(item);
            }
        }
    }
}

LevelState parse_record(const std::vector<std::uint8_t>& record,
                        LevelStorage storage) {
    require_range(0, kPreambleSize, record.size(), "level preamble");

    LevelState out{};
    out.storage = storage;
    out.writer_unowned_marker = record[0];
    for (std::size_t i = 0; i < out.boundaries.size(); ++i) {
        out.boundaries[i] = u32(record.data() + 1 + i * 4);
    }
    if (out.boundaries[0] != kPreambleSize ||
        !std::is_sorted(out.boundaries.begin(), out.boundaries.end())) {
        throw std::runtime_error("invalid level boundaries");
    }

    const auto header_at = static_cast<std::size_t>(out.boundaries[0]);
    require_range(header_at, kHeaderSize, record.size(), "level header");
    std::copy_n(record.begin() + static_cast<std::ptrdiff_t>(header_at),
                out.header.size(), out.header.begin());
    out.width = s16(out.header.data() + 0x001);
    out.height = s16(out.header.data() + 0x003);
    if (out.width <= 0 || out.height <= 0) {
        throw std::runtime_error("invalid level dimensions");
    }
    out.environment_profile = out.header[0x290];
    out.renderer_vertical_offset =
        static_cast<std::int8_t>(out.header[0x009]);
    out.renderer_fill_base_a = out.header[0x298];
    out.renderer_fill_base_b = out.header[0x299];
    out.render_threshold_floor = s16(out.header.data() + 0x29A);
    if (out.render_threshold_floor == 0) {
        // CODE.2 normalizes serialized zero to live value one immediately
        // after level load, while the raw header remains preserved above.
        out.render_threshold_floor = 1;
    }
    out.music_sequence = out.header[0x29C];

    const int family_a_count = s16(out.header.data() + 0x28A);
    const int family_c_count = s16(out.header.data() + 0x28C);
    const int family_b_count = s16(out.header.data() + 0x28E);
    if (family_a_count < 0 || family_b_count < 0 || family_c_count < 0) {
        throw std::runtime_error("negative level world-record count");
    }

    const auto cell_count = static_cast<std::size_t>(out.width) * out.height;
    if (cell_count > std::numeric_limits<std::size_t>::max() / 2) {
        throw std::runtime_error("level cell count overflow");
    }
    const auto grid_at = header_at + kHeaderSize;
    const auto expected_grid_end = grid_at + cell_count * 2;
    if (out.boundaries[1] != expected_grid_end) {
        throw std::runtime_error("level grid boundary mismatch");
    }
    require_range(grid_at, cell_count * 2, record.size(), "level cell grid");
    out.cell_words.resize(cell_count);
    for (std::size_t i = 0; i < cell_count; ++i) {
        out.cell_words[i] = u16(record.data() + grid_at + i * 2);
    }

    std::size_t pos = out.boundaries[1];
    if (storage == LevelStorage::ArchiveCompact) {
        read_list_group(record, pos, out.width, out.height,
                        out.group1, true, &out.group2);
        out.active_animation_count = 0;
    } else {
        read_list_group(record, pos, out.width, out.height,
                        out.group1, false, nullptr);
        require_range(pos, 1, record.size(), "active-animation byte");
        out.active_animation_count = record[pos++];
        read_list_group(record, pos, out.width, out.height,
                        out.group2, false, nullptr);
    }
    if (pos != out.boundaries[2]) {
        throw std::runtime_error("level 10-byte-list boundary mismatch");
    }

    const auto family_a_size = out.boundaries[3] - out.boundaries[2];
    const auto family_b_size = out.boundaries[4] - out.boundaries[3];
    const auto family_c_size = out.boundaries[5] - out.boundaries[4];
    if (storage == LevelStorage::ArchiveCompact) {
        if (family_a_size != static_cast<std::size_t>(family_a_count) * 11 ||
            family_b_size != static_cast<std::size_t>(family_b_count) * 11 ||
            family_c_size != static_cast<std::size_t>(family_c_count) * 11) {
            throw std::runtime_error("compact world-record family boundary mismatch");
        }
    } else {
        if (family_a_size != static_cast<std::size_t>(family_a_count) * 22 ||
            family_b_size != 2 + static_cast<std::size_t>(family_b_count) * 22 ||
            family_c_size != static_cast<std::size_t>(family_c_count) * 22) {
            throw std::runtime_error("direct world-record family boundary mismatch");
        }
        require_range(out.boundaries[3], 2, record.size(), "direct family-B count");
        if (s16(record.data() + out.boundaries[3]) != family_b_count) {
            throw std::runtime_error("direct family-B serialized count mismatch");
        }
    }

    out.family_a_raw = slice(record, out.boundaries[2], out.boundaries[3], "family A");
    out.family_b_raw = slice(record, out.boundaries[3], out.boundaries[4], "family B");
    out.family_c_raw = slice(record, out.boundaries[4], out.boundaries[5], "family C");

    if (out.boundaries[6] - out.boundaries[5] != kEventSize) {
        throw std::runtime_error("event-script table is not 0x514 bytes");
    }
    require_range(out.boundaries[5], kEventSize, record.size(), "event-script table");
    std::copy_n(record.begin() + static_cast<std::ptrdiff_t>(out.boundaries[5]),
                out.event_scripts.size(), out.event_scripts.begin());

    pos = out.boundaries[6];
    require_range(pos, 4, record.size(), "automap tail dimensions");
    const int tail_width = s16(record.data() + pos);
    const int tail_height = s16(record.data() + pos + 2);
    pos += 4;
    if (tail_width != out.width || tail_height != out.height) {
        throw std::runtime_error("automap tail dimensions disagree with header");
    }
    require_range(pos, cell_count, record.size(), "automap cache");
    out.automap_cache.assign(
        record.begin() + static_cast<std::ptrdiff_t>(pos),
        record.begin() + static_cast<std::ptrdiff_t>(pos + cell_count));
    pos += cell_count;

    require_range(pos, 4, record.size(), "automap annotation length");
    const auto annotation_size = static_cast<std::size_t>(u32(record.data() + pos));
    pos += 4;
    require_range(pos, annotation_size, record.size(), "automap annotations");
    out.automap_annotations.assign(
        record.begin() + static_cast<std::ptrdiff_t>(pos),
        record.begin() + static_cast<std::ptrdiff_t>(pos + annotation_size));
    pos += annotation_size;
    if (pos != record.size()) {
        throw std::runtime_error("unexpected bytes after automap annotation tail");
    }
    return out;
}

}  // namespace

PartyImage PartyImage::from_bytes(const std::vector<std::uint8_t>& source) {
    if (source.size() != kSize) {
        throw std::runtime_error("CHARS image must be exactly 0x920 bytes");
    }
    PartyImage out{};
    std::copy(source.begin(), source.end(), out.bytes.begin());
    return out;
}

std::string PartyImage::name(std::size_t actor) const {
    if (actor >= kActorCount) throw std::out_of_range("party actor index");
    const auto at = actor * kActorSize;
    std::size_t n = 0;
    while (n < 16 && bytes[at + n] != 0) ++n;
    return std::string(reinterpret_cast<const char*>(bytes.data() + at), n);
}

std::uint16_t PartyImage::status_word(std::size_t actor) const {
    if (actor >= kActorCount) throw std::out_of_range("party actor index");
    const auto at = actor * kActorSize + 0x10;
    return u16(bytes.data() + at);
}

bool PartyImage::active(std::size_t actor) const {
    return (status_word(actor) & 0x8000U) == 0;
}

LevelState ExplorationStateCodec::parse_archive_level(
    const std::vector<std::uint8_t>& levels_dat, int level_id) {
    require_range(0, 4, levels_dat.size(), "LEVELS.DAT header");
    const auto count = static_cast<std::size_t>(u32(levels_dat.data()));
    if (level_id < 0 || static_cast<std::size_t>(level_id) >= count) {
        throw std::out_of_range("LEVELS.DAT level id");
    }
    const auto table_size = 4 + count * 4;
    require_range(4, count * 4, levels_dat.size(), "LEVELS.DAT directory");
    const auto start = static_cast<std::size_t>(
        u32(levels_dat.data() + 4 + static_cast<std::size_t>(level_id) * 4));
    const auto end = static_cast<std::size_t>(
        static_cast<std::size_t>(level_id + 1) < count
            ? u32(levels_dat.data() + 4 + static_cast<std::size_t>(level_id + 1) * 4)
            : levels_dat.size());
    if (start < table_size || end <= start || end > levels_dat.size()) {
        throw std::runtime_error("invalid LEVELS.DAT record offsets");
    }
    return parse_record(slice(levels_dat, start, end, "LEVELS.DAT record"),
                        LevelStorage::ArchiveCompact);
}

LevelState ExplorationStateCodec::parse_direct_level(
    const std::vector<std::uint8_t>& lev_dat) {
    return parse_record(lev_dat, LevelStorage::DirectSave);
}

}  // namespace rloft
