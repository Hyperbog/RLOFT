#pragma once

#include <cstdint>
#include <optional>

#include "rloft/vga_planar_memory.hpp"

namespace rloft {

// Original CODE.2 2A078; includes only globals directly consumed
// by 2AB38/2AB6C and the graphics-controller mode register.
// All are observed runtime state, not inferred screen dimensions.
struct CombatOriginalVgaCopyState final {
    std::uint32_t selected_page_efe9{};
    std::uint32_t linear_page_base_f001{};
    std::uint32_t page_stride_f005{};
    std::uint32_t row_pitch_eff9{};
    std::uint32_t row_shift_efed{};
    std::uint32_t row_shift_eff1{};
    // GC index 05h is unknown until an observed port write.
    std::optional<std::uint8_t> graphics_mode_index05{};
};

struct CombatOriginalVgaCopyRect final {
    std::uint32_t destination_page{};
    std::uint32_t source_row{};
    std::uint32_t source_column{};
    std::uint32_t destination_row{};
    std::uint32_t destination_column{};
    std::uint32_t row_count{};
    std::uint32_t column_count{};
};

struct CombatOriginalVgaCopyResult final {
    std::uint32_t source_linear_address{};
    std::uint32_t destination_linear_address{};
    std::uint32_t original_bytes_per_row{};
    std::uint32_t vga_address_pairs_copied{};
    std::uint8_t destination_page_selected{};
    std::uint8_t source_page_restored{};
};

// Performs original read-mode-0 latch loading and WRITE MODE 1
// four-plane VGA byte copying via 2A078's REP MOVSB loop.
// Cannot emulate an original unbounded zero-height DEC loop or
// accesses outside the captured 64KiB aperture: returns nullopt
// WITHOUT changing captured host pixels or registers instead.
class CombatOriginalVgaCopyModel final {
public:
    [[nodiscard]] static std::optional<CombatOriginalVgaCopyResult>
    copy_2a078(
        VgaPlanarMemory& vga,
        CombatOriginalVgaCopyState& original_globals,
        const CombatOriginalVgaCopyRect& rect);
};

} // namespace rloft
