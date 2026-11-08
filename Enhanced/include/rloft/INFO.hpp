#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft {

// Original INFO.DAT layout:
//   0x00: 13 ASCII decimal resource-location selectors
//   0x0D: signed little-endian int16 path length
//   0x0F: exact path bytes (not NUL-terminated)
// CODE.1 and CODE.2 activate only RES0..RES10; selectors 11 and 12 are
// preserved without assigning them a runtime role.
struct InfoDat {
    static constexpr std::size_t kSelectorCount = 13;
    static constexpr std::size_t kActiveResCount = 11;

    std::array<std::uint8_t, kSelectorCount> resource_locations{};
    std::string resource_path; // Original DOS bytes; not a native filesystem path.
};

// Strict, lossless binary codec. Rejects short files, invalid ASCII digits,
// negative lengths, and unexpected trailing or missing bytes.
[[nodiscard]] InfoDat parse_info_dat(const std::vector<std::uint8_t>& bytes);
[[nodiscard]] std::vector<std::uint8_t> encode_info_dat(const InfoDat& info);

// Reconstruct original DOS resource names for analysis/compatibility only.
// Never resolve these historical drive paths on the native host filesystem.
[[nodiscard]] std::string info_legacy_res_path(const InfoDat& info,
                                               std::size_t archive_index);
[[nodiscard]] std::string info_legacy_numbered_path(const InfoDat& info,
                                                    unsigned resource_number);

} // namespace rloft
