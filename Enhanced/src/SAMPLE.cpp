#include "rloft/SAMPLE.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

constexpr std::size_t kDirectoryEntryBytes = 6U;
constexpr std::size_t kTimbreRecordBytes = 14U;

std::uint16_t read_u16(const std::vector<std::uint8_t>& b,
                       std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[at]) |
        (static_cast<unsigned>(b[at + 1U]) << 8U));
}

std::uint32_t read_u32(const std::vector<std::uint8_t>& b,
                       std::size_t at) noexcept {
    return static_cast<std::uint32_t>(b[at]) |
           (static_cast<std::uint32_t>(b[at + 1U]) << 8U) |
           (static_cast<std::uint32_t>(b[at + 2U]) << 16U) |
           (static_cast<std::uint32_t>(b[at + 3U]) << 24U);
}

void append_u16(std::vector<std::uint8_t>& out, std::uint16_t x) {
    out.push_back(static_cast<std::uint8_t>(x));
    out.push_back(static_cast<std::uint8_t>(x >> 8U));
}

void append_u32(std::vector<std::uint8_t>& out, std::uint32_t x) {
    for (unsigned shift = 0; shift < 32U; shift += 8U) {
        out.push_back(static_cast<std::uint8_t>(x >> shift));
    }
}

std::int8_t signed_byte(std::uint8_t value) noexcept {
    const int x = value <= 0x7FU ? static_cast<int>(value)
                                  : static_cast<int>(value) - 0x100;
    return static_cast<std::int8_t>(x);
}

struct DirectoryEntry {
    std::uint8_t patch{};
    std::uint8_t bank{};
    std::uint32_t offset{};
};

} // namespace

const SampleAdTimbre* SampleAdLibrary::find(
    std::uint8_t bank, std::uint8_t patch) const noexcept {
    for (const auto& timbre : timbres) {
        if (timbre.bank == bank && timbre.patch == patch) return &timbre;
    }
    return nullptr;
}

SampleAdLibrary parse_sample_ad(const std::vector<std::uint8_t>& bytes) {
    std::size_t at = 0;
    std::vector<DirectoryEntry> directory;
    while (true) {
        if (at > bytes.size() || bytes.size() - at < 2U) {
            throw std::runtime_error("SAMPLE.AD directory has no terminator");
        }
        const auto patch = bytes[at];
        const auto bank = bytes[at + 1U];
        if (patch == 0xFFU && bank == 0xFFU) {
            at += 2U;
            break;
        }
        if (bytes.size() - at < kDirectoryEntryBytes) {
            throw std::runtime_error("SAMPLE.AD truncated directory entry");
        }
        directory.push_back({patch, bank, read_u32(bytes, at + 2U)});
        at += kDirectoryEntryBytes;
    }
    if (directory.empty()) {
        throw std::runtime_error("SAMPLE.AD has no timbres");
    }

    SampleAdLibrary result{};
    result.timbres.reserve(directory.size());
    for (const auto& entry : directory) {
        if (entry.offset != at) {
            throw std::runtime_error("SAMPLE.AD noncontiguous or wrong timbre offset");
        }
        if (at > bytes.size() || bytes.size() - at < kTimbreRecordBytes ||
            read_u16(bytes, at) != kTimbreRecordBytes) {
            throw std::runtime_error("SAMPLE.AD invalid 14-byte OPL2 record");
        }
        const std::size_t p = at + 2U;
        result.timbres.push_back({
            entry.patch, entry.bank, signed_byte(bytes[p]),
            bytes[p+1U], bytes[p+2U], bytes[p+3U], bytes[p+4U],
            bytes[p+5U], bytes[p+6U], bytes[p+7U], bytes[p+8U],
            bytes[p+9U], bytes[p+10U], bytes[p+11U]
        });
        at += kTimbreRecordBytes;
    }
    if (at != bytes.size()) {
        throw std::runtime_error("SAMPLE.AD unexpected trailing data");
    }
    return result;
}

std::vector<std::uint8_t> encode_sample_ad(const SampleAdLibrary& library) {
    const auto count = library.timbres.size();
    const auto max_file = static_cast<std::size_t>(
        std::numeric_limits<std::uint32_t>::max());
    // Constrain final extent and each absolute directory offset to uint32.
    if (count == 0U || count > (max_file - 2U) /
        (kDirectoryEntryBytes + kTimbreRecordBytes)) {
        throw std::invalid_argument("SAMPLE.AD has invalid timbre count");
    }
    const auto header_size = kDirectoryEntryBytes * count + 2U;
    const auto total_size = header_size + kTimbreRecordBytes * count;
    std::vector<std::uint8_t> bytes;
    bytes.reserve(total_size);
    std::size_t offset = header_size;
    for (const auto& entry : library.timbres) {
        if (entry.patch == 0xFFU && entry.bank == 0xFFU) {
            throw std::invalid_argument("SAMPLE.AD entry conflicts with terminator");
        }
        bytes.push_back(entry.patch);
        bytes.push_back(entry.bank);
        append_u32(bytes, static_cast<std::uint32_t>(offset));
        offset += kTimbreRecordBytes;
    }
    bytes.push_back(0xFFU);
    bytes.push_back(0xFFU);
    for (const auto& t : library.timbres) {
        append_u16(bytes, static_cast<std::uint16_t>(kTimbreRecordBytes));
        bytes.push_back(static_cast<std::uint8_t>(t.transpose));
        bytes.insert(bytes.end(), {
            t.mod_avekm, t.mod_ksl_tl, t.mod_ad, t.mod_sr, t.mod_ws,
            t.fb_c, t.car_avekm, t.car_ksl_tl, t.car_ad, t.car_sr, t.car_ws
        });
    }
    return bytes;
}

} // namespace rloft
