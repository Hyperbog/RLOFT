#pragma once
// Host read-only RES directory / CODE.2 31334 indexing ABI.
// The retained guest owns allocation, decoded image cache and DOS streams.
// This adapter never copies or edits source game state.
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft::resource_cache {
using Byte=std::uint8_t;
struct Record {
    std::uint32_t start=0;
    std::uint32_t end=0;
    std::uint8_t type=0;
};
// Locate record in authenticated RES0..RES10 archive, fail-closed on
// missing index, incomplete directory, or truncated five-byte envelope.
[[nodiscard]] bool locate(const std::vector<Byte>& archive,
                          std::uint32_t index,Record& out) noexcept;
// Source-image registration: type 1 pictures, RES0 type 2 fonts,
// type 6/7 textures. All original registered payload guards retained.
[[nodiscard]] constexpr bool tracked_type(std::uint8_t type) noexcept {
    return type==1u || type==2u || type==6u || type==7u;
}
[[nodiscard]] bool authenticate(const std::vector<Byte>& archive,
                                const Record& record,
                                unsigned archive_slot) noexcept;
} // namespace rloft::resource_cache
