#include "rloft/combat_original_vga_backing.hpp"

#include "rloft/combat_original_vga_address.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {
namespace {
struct BackingLayout final {
    std::uint32_t physical_start{};
    std::uint8_t starting_plane{};
    std::uint32_t bytes_per_row{};
    std::uint32_t rows{};
    std::size_t buffer_start{};
    std::size_t total_bytes{};
};

std::optional<BackingLayout> prepare(
    const CombatOriginalDgroupCapture& memory,
    const CombatOriginalVgaCopyState& video,
    const CombatOriginalOverlayBackingRequest& args) noexcept {
    const auto start=CombatOriginalVgaAddressModel::inspect_2ab38(
        args.row_bits,args.column_bits,
        video.row_shift_efed,video.row_shift_eff1,
        video.linear_page_base_f001);

    // 2A13A..2A141 and 2A19F..2A1A7: original ADDs wrap
    // modulo 2^32; only SOURCE column's plane bits are used.
    const auto span=static_cast<std::uint32_t>(
        args.column_count_bits+
        static_cast<std::uint32_t>(start.original_plane_ecx));
    const auto bytes=static_cast<std::uint32_t>((span+3U)>>2U);
    const auto rows=args.row_count_bits;

    // Both routines run DEC/JNE after first iteration without
    // testing for zero rows. They cannot be emulated with an
    // unbounded host source; do not fabricate a safe DOS no-op.
    if(rows==0U || rows>VgaPlanarMemory::kApertureBytes)
        return std::nullopt;
    const auto plane_size=static_cast<std::uint64_t>(rows)*bytes;
    const auto total=4U*plane_size;
    if(plane_size>VgaPlanarMemory::kApertureBytes ||
       total>memory.bytes.size() ||
       args.backing_pointer<memory.captured_base_dgroup_offset)
        return std::nullopt;
    const auto relative=static_cast<std::uint64_t>(
        args.backing_pointer-memory.captured_base_dgroup_offset);
    if(relative>memory.bytes.size() ||
       total>memory.bytes.size()-relative)
        return std::nullopt;

    // Check *all* source/destination VGA addresses before any
    // register or byte writes. The original binary did not bounds
    // check DOS memory; incomplete captures are unsupported.
    auto vga_row=start.original_linear_address_ebx;
    for(std::uint32_t y=0U;y<rows;++y) {
        if(bytes!=0U) {
            if(vga_row<VgaPlanarMemory::kPhysicalBase)
                return std::nullopt;
            const auto relative_vga=static_cast<std::uint64_t>(
                vga_row-VgaPlanarMemory::kPhysicalBase);
            if(relative_vga>=VgaPlanarMemory::kApertureBytes ||
               bytes>VgaPlanarMemory::kApertureBytes-relative_vga)
                return std::nullopt;
        }
        // REP MOVSB advances bytes; add EFF9 - bytes afterward.
        // Its resulting next-row start is just start + EFF9.
        vga_row+=video.row_pitch_eff9;
    }

    return BackingLayout{
        start.original_linear_address_ebx,
        start.original_plane_ecx,bytes,rows,
        static_cast<std::size_t>(relative),
        static_cast<std::size_t>(total)};
}
} // namespace

std::optional<CombatOriginalVgaBackingResult>
CombatOriginalVgaBackingModel::capture_2a114(
    VgaPlanarMemory& vga,
    CombatOriginalDgroupCapture& dgroup,
    const CombatOriginalVgaCopyState& video,
    const CombatOriginalOverlayBackingRequest& args) {
    const auto layout=prepare(dgroup,video,args);
    if(!layout) return std::nullopt;
    std::size_t position=layout->buffer_start;
    for(std::uint8_t plane=0U;plane<4U;++plane) {
        // 2A14E AX=0004; 2A159 writes port 03CEh index 04h,
        // 2A169 increments AH. The observed selector remains 3.
        vga.set_graphics_read_map_04(plane);
        auto row=layout->physical_start;
        for(std::uint32_t y=0U;y<layout->rows;++y) {
            for(std::uint32_t x=0U;x<layout->bytes_per_row;++x)
                dgroup.bytes[position++]=vga.read(row+x,plane);
            row+=video.row_pitch_eff9;
        }
    }
    return CombatOriginalVgaBackingResult{
        layout->physical_start,layout->starting_plane,
        layout->bytes_per_row,
        static_cast<std::uint32_t>(layout->total_bytes)};
}

std::optional<CombatOriginalVgaBackingResult>
CombatOriginalVgaBackingModel::restore_2a178(
    VgaPlanarMemory& vga,
    const CombatOriginalDgroupCapture& dgroup,
    const CombatOriginalVgaCopyState& video,
    const CombatOriginalOverlayBackingRequest& args) {
    const auto layout=prepare(dgroup,video,args);
    if(!layout) return std::nullopt;
    std::size_t position=layout->buffer_start;
    for(std::uint8_t plane=0U;plane<4U;++plane) {
        const auto mask=static_cast<std::uint8_t>(1U<<plane);
        // 2A1B4 AX=0102; 2A1BF selects map-mask plane; 2A1CF
        // doubles AH for the next block of backing bytes.
        vga.set_sequencer_map_mask_02(mask);
        auto row=layout->physical_start;
        for(std::uint32_t y=0U;y<layout->rows;++y) {
            for(std::uint32_t x=0U;x<layout->bytes_per_row;++x)
                vga.write(row+x,mask,dgroup.bytes[position++]);
            row+=video.row_pitch_eff9;
        }
    }
    // 2A1DA..2A1DC OUT 03C4h,AX=FF02h:
    // all four physical VGA planes remain enabled.
    vga.set_sequencer_map_mask_02(0x0FU);
    return CombatOriginalVgaBackingResult{
        layout->physical_start,layout->starting_plane,
        layout->bytes_per_row,
        static_cast<std::uint32_t>(layout->total_bytes)};
}

} // namespace rloft
