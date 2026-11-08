#include "rloft/combat_original_vga_copy.hpp"

#include "rloft/combat_irq_display_runtime.hpp"
#include "rloft/combat_original_vga_address.hpp"

#include <array>
#include <cstdint>
#include <optional>

namespace rloft {
namespace {
bool inside_aperture(std::uint32_t address) noexcept {
    return address>=VgaPlanarMemory::kPhysicalBase &&
        address-VgaPlanarMemory::kPhysicalBase<
            VgaPlanarMemory::kApertureBytes;
}

bool captured_addresses(
    std::uint32_t source,
    std::uint32_t destination,
    std::uint32_t bytes_per_row,
    std::uint32_t rows,
    std::uint32_t pitch) noexcept {
    const auto row_delta=static_cast<std::uint32_t>(
        pitch-bytes_per_row);
    for(std::uint32_t row=0U;row<rows;++row) {
        for(std::uint32_t col=0U;col<bytes_per_row;++col) {
            if(!inside_aperture(source) ||
               !inside_aperture(destination)) return false;
            ++source;
            ++destination;
        }
        source+=row_delta;
        destination+=row_delta;
    }
    return true;
}
} // namespace

std::optional<CombatOriginalVgaCopyResult>
CombatOriginalVgaCopyModel::copy_2a078(
    VgaPlanarMemory& vga,
    CombatOriginalVgaCopyState& globals,
    const CombatOriginalVgaCopyRect& rect) {
    // 2A081..2A08E: 2AB38 captures source physical address
    // and source column plane using the current original F001.
    const auto source=CombatOriginalVgaAddressModel::inspect_2ab38(
        rect.source_row,rect.source_column,
        globals.row_shift_efed,globals.row_shift_eff1,
        globals.linear_page_base_f001);
    // 2A091..2A09A: page selection ANDs EAX with FFh.
    // The caller's old EFE9 LOW byte is saved in AH for restore.
    const auto old_page=static_cast<std::uint8_t>(
        globals.selected_page_efe9);
    const auto dest_page=static_cast<std::uint8_t>(
        rect.destination_page);
    const auto destination_page_state=CombatVgaPageSelectionModel::inspect(
        dest_page,globals.page_stride_f005);
    const auto destination=CombatOriginalVgaAddressModel::inspect_2ab38(
        rect.destination_row,rect.destination_column,
        globals.row_shift_efed,globals.row_shift_eff1,
        destination_page_state.original_f001_vga_linear_base);

    // 2A08E ADD [EBP+20h], ECX and 2A0D0..2A0D3:
    // the SOURCE start-plane offset extends the byte span.
    // The destination column's plane bits are NOT used.
    const auto span=static_cast<std::uint32_t>(
        rect.column_count+
        static_cast<std::uint32_t>(source.original_plane_ecx));
    const auto bytes_per_row=static_cast<std::uint32_t>(
        (span+3U)>>2U);

    // Original 2A0E6..2A0EF is DEC EAX/JNE with no zero
    // height guard. Reject unbounded host captures.
    if(rect.row_count==0U ||
       static_cast<std::uint64_t>(rect.row_count)*
           bytes_per_row>VgaPlanarMemory::kApertureBytes ||
       rect.row_count>VgaPlanarMemory::kApertureBytes ||
       !captured_addresses(source.original_linear_address_ebx,
                           destination.original_linear_address_ebx,
                           bytes_per_row,rect.row_count,
                           globals.row_pitch_eff9))
        return std::nullopt;

    // The original sequence selects the destination page for
    // 2AB38, then 2AB6C RESTORES the previous EFE9 page
    // before the video-port writes/copy. 2AB6C recalculates F001.
    const auto restored=CombatVgaPageSelectionModel::inspect(
        old_page,globals.page_stride_f005);
    globals.selected_page_efe9=restored.original_efe9_page;
    globals.linear_page_base_f001=
        restored.original_f001_vga_linear_base;

    // 2A0BD OUT 03CEh,4105h selects VGA write mode 1.
    // 2A0C7 OUT 03C4h,0F02h enables all four planes.
    globals.graphics_mode_index05=0x41U;
    vga.set_sequencer_map_mask_02(0x0FU);
    const auto row_delta=static_cast<std::uint32_t>(
        globals.row_pitch_eff9-bytes_per_row);
    auto src=source.original_linear_address_ebx;
    auto dst=destination.original_linear_address_ebx;
    for(std::uint32_t row=0U;row<rect.row_count;++row) {
        for(std::uint32_t col=0U;col<bytes_per_row;++col) {
            // 2A0E8 REP MOVSB in GC write mode 1:
            // VGA read loads all FOUR hardware latch bytes.
            // The subsequent VGA write ignores the CPU color byte
            // and copies each original latch plane to destination.
            // Read immediately before each write; overlapping
            // regions therefore follow original FORWARD MOVSB.
            std::array<std::uint8_t,4> latched{};
            for(std::uint8_t plane=0U;plane<4U;++plane)
                latched[plane]=vga.read(src,plane);
            for(std::uint8_t plane=0U;plane<4U;++plane)
                vga.write(dst,static_cast<std::uint8_t>(1U<<plane),
                          latched[plane]);
            ++src;
            ++dst;
        }
        src+=row_delta;
        dst+=row_delta;
    }

    // 2A0F6 OUT 03CEh,4005h restores graphics mode 40h.
    // The sequencer's 0Fh plane mask remains selected.
    globals.graphics_mode_index05=0x40U;

    CombatOriginalVgaCopyResult out{};
    out.source_linear_address=source.original_linear_address_ebx;
    out.destination_linear_address=destination.original_linear_address_ebx;
    out.original_bytes_per_row=bytes_per_row;
    out.vga_address_pairs_copied=rect.row_count*bytes_per_row;
    out.destination_page_selected=dest_page;
    out.source_page_restored=old_page;
    return out;
}

} // namespace rloft
