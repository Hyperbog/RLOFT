#include "rloft/CODE2.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kFileSize = 0x88661U;
constexpr std::size_t kModule = 0x25D24U;
constexpr std::size_t kLe = 0x27BDCU;
constexpr std::size_t kCodeBase = 0x3AF24U;
constexpr std::size_t kDataBase = 0x75F24U;
constexpr std::size_t kPortraitOffset = 0xCF70U;
constexpr std::size_t kMapBytes = 6U * 256U;

std::uint32_t le32(const std::vector<std::uint8_t>& b,
                   std::size_t offset) {
    if (offset > b.size() || b.size() - offset < 4U) {
        throw std::runtime_error("CODE.2 truncated LE field");
    }
    return std::uint32_t(b[offset]) |
           (std::uint32_t(b[offset + 1U]) << 8U) |
           (std::uint32_t(b[offset + 2U]) << 16U) |
           (std::uint32_t(b[offset + 3U]) << 24U);
}

void validate_layout(const std::vector<std::uint8_t>& b) {
    if (b.size() != kFileSize ||
        b[0] != 'M' || b[1] != 'Z' ||
        b[kModule] != 'M' || b[kModule + 1U] != 'Z' ||
        kModule + le32(b, kModule + 0x3CU) != kLe ||
        b[kLe] != 'L' || b[kLe + 1U] != 'E' ||
        le32(b, kLe + 0x14U) != 0x4EU ||
        le32(b, kLe + 0x18U) != 1U ||
        le32(b, kLe + 0x1CU) != 0x353A0U ||
        le32(b, kLe + 0x28U) != 0x1000U ||
        le32(b, kLe + 0x40U) != 0xC4U ||
        le32(b, kLe + 0x44U) != 2U ||
        kModule + le32(b, kLe + 0x80U) != kCodeBase) {
        throw std::runtime_error("CODE.2 LE executable layout mismatch");
    }
    const std::size_t objects = kLe + 0xC4U;
    if (le32(b, objects) != 0x3A852U ||
        le32(b, objects + 4U) != 0x10000U ||
        le32(b, objects + 12U) != 1U ||
        le32(b, objects + 16U) != 0x3BU ||
        le32(b, objects + 24U) != 0x2DD10U ||
        le32(b, objects + 28U) != 0x50000U ||
        le32(b, objects + 36U) != 0x3CU ||
        le32(b, objects + 40U) != 0x13U ||
        kCodeBase + 0x3BU * 0x1000U != kDataBase ||
        b.size() - kDataBase < kPortraitOffset + kMapBytes) {
        throw std::runtime_error("CODE.2 data-object geometry mismatch");
    }
}

Code2PortraitMaps read_maps(const std::vector<std::uint8_t>& b) {
    const std::size_t start = kDataBase + kPortraitOffset;
    if (start > b.size() || b.size() - start < kMapBytes) {
        throw std::runtime_error("CODE.2 portrait remap table truncated");
    }
    Code2PortraitMaps maps{};
    for (std::size_t i = 0; i < maps.size(); ++i) {
        std::copy_n(b.begin() + static_cast<std::ptrdiff_t>(
                        start + i * 256U), 256U, maps[i].begin());
    }
    return maps;
}
}  // namespace

Code2Executable::Code2Executable(std::vector<std::uint8_t> original)
    : bytes_(std::move(original)) {
    validate_layout(bytes_);
}

std::vector<std::uint8_t> Code2Executable::data_bytes(
    std::size_t offset, std::size_t count) const {
    const auto length = bytes_.size() - kDataBase;
    if (offset > length || count > length - offset) {
        throw std::out_of_range("CODE.2 DGROUP displacement not file-backed");
    }
    const auto start = kDataBase + offset;
    return {bytes_.begin() + static_cast<std::ptrdiff_t>(start),
            bytes_.begin() + static_cast<std::ptrdiff_t>(start + count)};
}

std::uint8_t Code2Executable::data_u8(std::size_t offset) const {
    return data_bytes(offset, 1U)[0];
}

std::uint16_t Code2Executable::data_u16(std::size_t offset) const {
    const auto b = data_bytes(offset, 2U);
    return static_cast<std::uint16_t>(b[0] | (unsigned(b[1]) << 8U));
}

Code2PortraitMaps Code2Executable::portrait_maps() const {
    return read_maps(bytes_);
}

Code2PortraitMaps Code2Executable::portrait_maps_from_raw(
    const std::vector<std::uint8_t>& original) {
    return read_maps(original);
}

}  // namespace rloft
