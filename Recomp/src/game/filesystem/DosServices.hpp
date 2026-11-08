#pragma once
// Strict DOS filename adapter used at the original CODE.2 AH=3Dh callback.
// Validated path bytes are still read from the same guest via seg8.
#include <cstdint>
#include <string>
#include <string_view>

namespace rloft::host_dos_services {
enum class CinematicPath:std::uint8_t {
    Valid, PrefixMismatch, FilenameMismatch
};
[[nodiscard]] CinematicPath validate_cinematic(
    std::string_view original_path,std::string_view mounted_prefix) noexcept;
// Original COLORs. special-case applies only at the observed DGROUP:CFC
// call site; never rewrite another DOS pathname.
[[nodiscard]] std::string canonical_basename(
    std::string name,std::uint32_t guest_path);
} // namespace rloft::host_dos_services
