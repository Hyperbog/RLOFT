#include "rloft/RES8.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

std::uint16_t read16(const std::vector<std::uint8_t>& b,
                     std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[at]) |
        (static_cast<unsigned>(b[at + 1U]) << 8U));
}

void write32(std::vector<std::uint8_t>& b,
             std::size_t at, std::uint32_t value) noexcept {
    for (unsigned i = 0; i < 4U; ++i) {
        b[at + i] = static_cast<std::uint8_t>(value >> (i * 8U));
    }
}

void check_res8_entry(const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() < 13U || bytes[0] != 'E' || bytes[1] != 'H' ||
        bytes[2] != 1U || read16(bytes, 3U) != 8U ||
        read16(bytes, 5U) == 0U || read16(bytes, 5U) > 0x7FFFU ||
        read16(bytes, 7U) == 0U || read16(bytes, 7U) > 0x7FFFU ||
        bytes[9U] == 0U || (bytes[11U] != 0U && bytes[11U] != 3U)) {
        throw std::runtime_error("RES8 requires a supported EH type-1 raster");
    }
    const auto expected = static_cast<std::size_t>(read16(bytes, 5U)) *
        static_cast<std::size_t>(read16(bytes, 7U)) *
        static_cast<std::size_t>(bytes[9U]) * 8U;
    if (bytes[11U] == 0U && bytes.size() - 13U != expected) {
        throw std::runtime_error("RES8 raw raster size disagrees with descriptor");
    }
}

std::vector<std::uint8_t> single_entry_archive(
    const std::vector<std::uint8_t>& entry) {
    std::vector<std::uint8_t> bytes(8U + entry.size());
    write32(bytes, 0U, 1U);
    write32(bytes, 4U, 8U);
    std::copy(entry.begin(), entry.end(), bytes.begin() + 8);
    return bytes;
}

} // namespace

Res8Archive::Res8Archive(std::vector<std::uint8_t> bytes)
    : original_(std::make_unique<ResArchive>(std::move(bytes))) {
    if (original_->entry_count() <= 0) {
        throw std::runtime_error("RES8 contains no environment images");
    }
    overrides_.resize(static_cast<std::size_t>(original_->entry_count()));
    for (int i = 0; i < original_->entry_count(); ++i) {
        check_res8_entry(original_->entry(i));
    }
}

Res8Archive::~Res8Archive() = default;
Res8Archive::Res8Archive(Res8Archive&&) noexcept = default;
Res8Archive& Res8Archive::operator=(Res8Archive&&) noexcept = default;

int Res8Archive::entry_count() const noexcept {
    return original_->entry_count();
}

std::vector<std::uint8_t> Res8Archive::entry(int index) const {
    if (index < 0 || index >= entry_count()) {
        throw std::out_of_range("RES8 environment image index");
    }
    const auto& replacement = overrides_[static_cast<std::size_t>(index)];
    return replacement.empty() ? original_->entry(index) : replacement;
}

Type1Raster Res8Archive::decode_type1(int index) const {
    if (index < 0 || index >= entry_count()) {
        throw std::out_of_range("RES8 environment image index");
    }
    const auto& replacement = overrides_[static_cast<std::size_t>(index)];
    if (replacement.empty()) return original_->decode_type1(index);
    ResArchive updated(single_entry_archive(replacement));
    return updated.decode_type1(0);
}

void Res8Archive::replace_pixels(
    int index, const std::vector<std::uint8_t>& pixels) {
    const auto decoded = decode_type1(index);
    if (pixels.size() != decoded.pixels.size()) {
        throw std::invalid_argument("RES8 replacement raster dimensions changed");
    }
    auto raw = entry(index);
    // All data bytes start after EH(5) + type-1 descriptor(8). Original
    // header fields (including transparent index and vertical transpose)
    // are retained, changing only compression to supported method 0.
    raw[11U] = 0U;
    raw.resize(13U);
    raw.insert(raw.end(), pixels.begin(), pixels.end());
    overrides_[static_cast<std::size_t>(index)] = std::move(raw);
}

std::vector<std::uint8_t> Res8Archive::encode() const {
    const auto count = overrides_.size();
    const auto limit = static_cast<std::size_t>(
        std::numeric_limits<std::uint32_t>::max());
    if (count > (limit - 4U) / 4U) {
        throw std::overflow_error("RES8 directory exceeds 32-bit offsets");
    }
    std::vector<std::vector<std::uint8_t>> entries;
    entries.reserve(count);
    std::size_t next = 4U + count * 4U;
    for (std::size_t i = 0; i < count; ++i) {
        auto raw = entry(static_cast<int>(i));
        if (raw.size() > limit - next) {
            throw std::overflow_error("RES8 image offsets exceed 32 bits");
        }
        next += raw.size();
        entries.push_back(std::move(raw));
    }
    std::vector<std::uint8_t> bytes(next);
    write32(bytes, 0U, static_cast<std::uint32_t>(count));
    std::size_t pos = 4U + count * 4U;
    for (std::size_t i = 0; i < count; ++i) {
        write32(bytes, 4U + i * 4U, static_cast<std::uint32_t>(pos));
        std::copy(entries[i].begin(), entries[i].end(),
                  bytes.begin() + static_cast<std::ptrdiff_t>(pos));
        pos += entries[i].size();
    }
    return bytes;
}

} // namespace rloft
