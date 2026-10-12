#pragma once
#include "rloft_native/native_sintab_raster.hpp"
#include "rloft_native/native_type7_scene_shape.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Read-only masks from the original CODE.2 LE object 2 at physical file
// offset 0x75F24 + C668 / C688 / C6A8. Not a replacement lookup invented
// from a modern framebuffer layout.
struct NativeType7VgaMasks {
    std::array<std::array<std::uint8_t,32>,3> tables{};
    bool initial_individual_columns{};
    std::uint8_t initial_video_page{};
    std::string original_code2_sha256;

    [[nodiscard]] std::array<std::uint8_t,3> for_run(
        std::uint32_t x,std::uint8_t width) const;
};
[[nodiscard]] NativeType7VgaMasks decode_native_type7_vga_masks(
    const std::vector<std::uint8_t>& original_code2);

struct NativeType7VgaCall {
    std::uint16_t viewport_column{};
    std::size_t plane_byte_address{}; // 0..65535 of VGA A0000 page window
    std::uint32_t ecx_lookup{};
    std::uint8_t sequencer_mask{};
    std::uint8_t pass_index{}; // 0,1,2, original conditional pass order
};

struct NativeType7VgaDispatch {
    std::vector<NativeType7VgaCall> calls;
    std::uint32_t lookup_after_dispatch{};
    std::uint8_t original_video_page{};
    bool individual_columns{};
    bool original_scene_projection_verified{false};
};

// Reconstructs 36B14 + 369D8's source-proven VGA mask and
// per-destination-column/group selection only. x/width must be clipped
// to the original 320-pixel logical viewport *before* this function.
// Mode=false: one grouped 369D8 call (width<=7) followed by XCHG AL,AH.
// Mode=true: each column width=1; swap ECX low bytes on odd column,
// original lookup is restored afterward.
// renderer_row and renderer_row_byte_offset are caller-supplied;
/// neither is inferred from a DOS camera state here.
[[nodiscard]] NativeType7VgaDispatch plan_native_type7_vga_dispatch(
    const NativeType7VgaMasks& masks,
    unsigned start_x,std::uint8_t width,
    std::uint8_t video_page,unsigned renderer_row,
    unsigned renderer_row_byte_offset,std::uint32_t ecx_lookup,
    bool individual_columns_mode);

struct NativeType7VgaDraw {
    std::size_t descriptor_invocations{};
    std::size_t raster_call_invocations{};
    std::size_t indexed_source_reads{};
    std::size_t masked_vga_writes{};
    std::size_t individual_plane_writes{};
    std::string resulting_four_plane_sha256;
    bool original_dos_frame_parity_verified{false};
};

// The original 369D8 call owns ALL descriptors in one EH7 source
// column: for each destination/group, traverse descriptors in order,
// and within each descriptor call original plane passes in order.
// Stages full four-plane result; any later malformed source or
// destination discards earlier staged pixel writes.
[[nodiscard]] NativeType7VgaDraw draw_native_type7_vga_dispatch(
    const NativeSintabPrograms& tables,
    const NativeType7SceneShape& shape,
    unsigned scale_index,unsigned source_column_index,
    const std::vector<std::uint8_t>& decoded_eh7_body,
    std::int64_t base_source_offset,
    const NativeType7VgaDispatch& dispatch,
    NativeIndexedVgaPlanes& target);

} // namespace rloft_native
