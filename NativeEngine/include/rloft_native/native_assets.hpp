#pragma once

#include "rloft_native/native_media.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Original resource archive: 32-bit entry count, contiguous 32-bit offset
// table and payload records. Parsed independently of the decomp demo.
class NativeResArchive final {
public:
    explicit NativeResArchive(std::vector<std::uint8_t> raw);
    [[nodiscard]] std::size_t entry_count() const noexcept { return offsets_.size(); }
    [[nodiscard]] std::vector<std::uint8_t> entry(std::size_t index) const;
private:
    std::vector<std::uint8_t> bytes_;
    std::vector<std::uint32_t> offsets_;
};

struct NativeType1Raster {
    std::uint16_t frames{};
    std::uint16_t width{};
    std::uint16_t height{};
    std::uint8_t transparent_index{};
    // Indexed pixels, frame-major, row-major, 8 pixels per width unit.
    std::vector<std::uint8_t> pixels;
};

// Original EH type-1 indexed image: independent decode of raw and CODE.2
// LZSS-4K compressed raster bodies. Invalid compressed data fails closed.
// This is an asset decoder, NOT the decomp demo's raster presenter.
[[nodiscard]] NativeType1Raster decode_native_type1_raster(
    const std::vector<std::uint8_t>& entry);

struct NativePalette6 {
    std::array<std::uint8_t, 768> rgb{};
};

// Original COLORS stores one byte bank-count followed by 768 bytes per bank.
// Values in RGB DAC banks must fit the original 6-bit range (0..63).
[[nodiscard]] NativePalette6 decode_native_palette(
    const std::vector<std::uint8_t>& colors, std::size_t bank);

struct NativeAssetCheck {
    std::string name;
    bool present{};
    bool validated{};
    std::uint64_t bytes{};
    std::size_t res_entries{};
    std::string issue;
};

struct NativeAssetReport {
    NativeMediaKind media_kind{};
    std::size_t indexed_files{};
    std::vector<NativeAssetCheck> checks;
    std::size_t valid{};
    std::size_t missing{};
    std::size_t invalid{};
};

// Strict read-only examination of real original data. No extraction, no
// sandbox/asset install writes, no original executable code, no gameplay.
// Validation is per-resource, so a broken optional asset does not prevent
// reporting the remaining files.
[[nodiscard]] NativeAssetReport inspect_native_assets(const NativeMedia& media);

} // namespace rloft_native
