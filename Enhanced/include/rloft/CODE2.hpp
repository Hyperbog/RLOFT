#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Original CODE.2 DGROUP CF70: six 256-byte portrait palette-remap tables.
// State 2 is the identity mapping in the authenticated retail executable.
using Code2PortraitMaps = std::array<std::array<std::uint8_t, 256>, 6>;

// Checked read-only view of the DOS/4GW-bound CODE.2 LE application data.
// Gameplay is owned by Enhanced; this is not another executable or VM.
// File-backed DGROUP values are indexed by original object-2 displacement.
class Code2Executable final {
public:
    explicit Code2Executable(std::vector<std::uint8_t> original);

    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept {
        return bytes_;
    }
    [[nodiscard]] std::uint8_t data_u8(std::size_t offset) const;
    [[nodiscard]] std::uint16_t data_u16(std::size_t offset) const;
    [[nodiscard]] std::vector<std::uint8_t> data_bytes(
        std::size_t offset, std::size_t count) const;
    [[nodiscard]] Code2PortraitMaps portrait_maps() const;

    // Preserves the old synthetic HUD fixtures which provide only the
    // physical portrait-table span, not a complete bound executable.
    [[nodiscard]] static Code2PortraitMaps portrait_maps_from_raw(
        const std::vector<std::uint8_t>& original);

private:
    std::vector<std::uint8_t> bytes_;
};

}  // namespace rloft
