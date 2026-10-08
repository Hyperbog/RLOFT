#include "rloft/combat_original_clipped_raster.hpp"
#include "rloft/combat_original_vga_address.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {
namespace {
std::uint32_t dword(const std::vector<std::uint8_t>& b,
                    std::size_t at) noexcept {
    return static_cast<std::uint32_t>(b[at])|
        (static_cast<std::uint32_t>(b[at+1U])<<8U)|
        (static_cast<std::uint32_t>(b[at+2U])<<16U)|
        (static_cast<std::uint32_t>(b[at+3U])<<24U);
}
std::uint16_t word(const std::vector<std::uint8_t>& b,
                   std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[at])|
        (static_cast<unsigned>(b[at+1U])<<8U));
}
std::uint8_t rol(std::uint8_t value,unsigned n) noexcept {
    n &= 7U;
    return static_cast<std::uint8_t>(
        (static_cast<unsigned>(value)<<n)|
        (static_cast<unsigned>(value)>>(8U-n)));
}
bool video_address(std::uint32_t p) noexcept {
    return p>=VgaPlanarMemory::kPhysicalBase &&
        p-VgaPlanarMemory::kPhysicalBase<VgaPlanarMemory::kApertureBytes;
}
std::optional<std::uint8_t> read_source(
    const CombatOriginalSpriteMemory& src,
    std::uint32_t original_address) noexcept {
    if(original_address<src.captured_base_address)return std::nullopt;
    const auto at=static_cast<std::uint64_t>(
        original_address-src.captured_base_address);
    if(at>=src.bytes.size())return std::nullopt;
    return src.bytes[static_cast<std::size_t>(at)];
}
struct Write final {
    std::uint32_t address{};
    std::uint8_t mask{};
    std::uint8_t color{};
    bool transparent{};
};
struct Pass final {
    std::uint8_t raw_ah{};
    std::vector<Write> writes;
};
} // namespace

std::optional<CombatOriginalClippedRasterResult>
CombatOriginalClippedRasterModel::draw_2a6b0(
    VgaPlanarMemory& vga,
    const CombatOriginalSpriteMemory& sprite,
    const CombatOriginalDgroupCapture& dgroup,
    std::uint32_t frame_bits,
    std::uint32_t row,
    std::uint32_t column,
    std::uint32_t scratch_pointer,
    const CombatOriginalSpriteVideoState& video,
    CombatOriginalClippedRasterState& original_state,
    CombatOriginalSpriteMachineState* mutable_code_image) {
    if(scratch_pointer<dgroup.captured_base_dgroup_offset)
        return std::nullopt;
    const auto scratch=static_cast<std::uint64_t>(
        scratch_pointer-dgroup.captured_base_dgroup_offset);
    if(scratch>dgroup.bytes.size() ||
       dgroup.bytes.size()-scratch<24U)return std::nullopt;
    const auto at=static_cast<std::size_t>(scratch);
    const auto skipped_top=dword(dgroup.bytes,at);
    const auto skipped_bottom=dword(dgroup.bytes,at+4U);
    const auto rows=dword(dgroup.bytes,at+8U);
    const auto skipped_left=dword(dgroup.bytes,at+12U);
    const auto skipped_right=dword(dgroup.bytes,at+16U);
    const auto columns=dword(dgroup.bytes,at+20U);
    if(sprite.resource_header_address<sprite.captured_base_address)
        return std::nullopt;
    const auto hoff=static_cast<std::uint64_t>(
        sprite.resource_header_address-sprite.captured_base_address);
    if(hoff>sprite.bytes.size() || sprite.bytes.size()-hoff<8U)
        return std::nullopt;
    const auto header=static_cast<std::size_t>(hoff);
    const auto stride=word(sprite.bytes,header);
    const auto transparent=(sprite.bytes[header+7U]&0x80U)!=0U;
    auto source_pointer=static_cast<std::uint32_t>(
        sprite.resource_header_address+8U+frame_bits*stride);
    const auto frame_begin=source_pointer;
    std::uint8_t key=0xFFU;
    if(transparent){
        const auto value=read_source(sprite,source_pointer);
        if(!value)return std::nullopt;
        key=*value;
        ++source_pointer;
    }
    // 2A7A4 self-patches the comparison immediate BEFORE pixel reads.
    // Even a later unsupported bounded capture leaves that code byte.
    original_state.code_comparison_byte_2a816=key;
    if(mutable_code_image!=nullptr &&
       !CombatOriginalSpriteCodePatchModel::patch_2a6b0(
            *mutable_code_image,key))
        return std::nullopt;
    CombatOriginalClippedRasterState s{};
    s.top_skip_bytes_f00d=skipped_top>>2U;
    s.bottom_skip_bytes_f011=skipped_bottom>>2U;
    s.visible_rows_f015=rows;
    s.left_skip_bytes_f019=(skipped_left+3U)>>2U;
    s.mod_four_f025=skipped_right&3U;
    s.right_flag_f029=skipped_right==0U?0U:1U;
    s.right_skip_bytes_f01d=skipped_right==0U?0U:(skipped_right>>2U);
    s.visible_plane_bytes_f021=static_cast<std::uint32_t>(
        (columns+s.mod_four_f025)>>2U);
    if(s.visible_plane_bytes_f021==0U){
        s.visible_plane_bytes_f021=1U;
        if(s.right_flag_f029==0U){
            s.initial_skipped_planes_f02d=4U-columns;
            --s.left_skip_bytes_f019;
        }
    }
    s.mod_four_f025=skipped_left&3U;
    const auto address=CombatOriginalVgaAddressModel::inspect_2ab38(
        row,column,video.first_row_shift_efed,
        video.second_row_shift_eff1,video.page_base_f001);
    const auto align=static_cast<std::uint32_t>(
        (static_cast<std::uint32_t>(address.original_plane_ecx)+4U-
         s.mod_four_f025)&3U);
    auto ah=rol(0x11U,align);
    const auto stride_per_plane=static_cast<std::uint32_t>(stride)>>2U;
    s.remaining_plane_passes_f031=4U;
    if(s.initial_skipped_planes_f02d>4U)return std::nullopt;
    for(std::uint32_t i=0;i<s.initial_skipped_planes_f02d;++i){
        source_pointer+=stride_per_plane;
        --s.remaining_plane_passes_f031;
        ah=rol(ah,1U);
    }
    // Original DEC/JNE on the two counters has no zero guard.
    // We cannot safely replay its unbounded memory traffic.
    if(s.remaining_plane_passes_f031==0U || rows==0U ||
       rows>VgaPlanarMemory::kApertureBytes)return std::nullopt;

    CombatOriginalClippedRasterResult result{};
    result.transparent_resource=transparent;
    std::vector<Pass> passes;
    passes.reserve(s.remaining_plane_passes_f031);
    auto destination=address.original_linear_address_ebx;
    auto row_remainder=static_cast<std::uint32_t>(
        video.row_pitch_eff9-s.visible_plane_bytes_f021-
        s.right_skip_bytes_f01d);
    std::uint32_t remaining=s.remaining_plane_passes_f031;
    // A source pass may have a different width because VGA plane
    // carry adjusts live F019/F01D/F021 on the next pass.
    while(remaining!=0U){
        const auto visible=s.visible_plane_bytes_f021;
        if(visible==0U ||
           static_cast<std::uint64_t>(visible)*rows>
                VgaPlanarMemory::kApertureBytes)return std::nullopt;
        Pass pass{};
        pass.raw_ah=ah;
        pass.writes.reserve(static_cast<std::size_t>(visible)*rows);
        auto src=static_cast<std::uint32_t>(
            source_pointer+s.top_skip_bytes_f00d);
        auto dst=destination;
        for(std::uint32_t y=0U;y<rows;++y){
            src+=s.left_skip_bytes_f019;
            for(std::uint32_t x=0U;x<visible;++x){
                if(!video_address(dst))return std::nullopt;
                const auto value=read_source(sprite,src);
                if(!value)return std::nullopt;
                pass.writes.push_back(Write{
                    dst,static_cast<std::uint8_t>(ah&0x0FU),
                    *value,*value==key});
                ++src;
                ++dst;
            }
            src+=s.right_skip_bytes_f01d;
            dst+=s.right_skip_bytes_f01d;
            dst+=row_remainder;
        }
        source_pointer=static_cast<std::uint32_t>(
            src+s.bottom_skip_bytes_f011);
        passes.push_back(std::move(pass));
        --remaining;
        if(remaining==0U)break;
        const bool carry=(ah&0x80U)!=0U;
        ah=rol(ah,1U);
        if(carry){
            ++destination;
            if(s.left_skip_bytes_f019!=0U){
                --s.left_skip_bytes_f019;
                --row_remainder;
                --destination;
                if(s.right_flag_f029!=0U)
                    ++s.right_skip_bytes_f01d;
                else
                    ++s.visible_plane_bytes_f021;
            } else if(s.right_flag_f029!=0U){
                ++s.right_skip_bytes_f01d;
                --s.visible_plane_bytes_f021;
                if(s.visible_plane_bytes_f021==0U)break;
            }
        }
    }
    // All original source/vga accesses are now bounded and verified:
    // write real planar bytes in the original OUT/move order.
    s.live_row_counter_f009=rows;
    s.code_comparison_byte_2a816=key;
    for(const auto& pass:passes){
        vga.write_sequencer_raw_mask_02(pass.raw_ah);
        ++result.plane_passes;
        result.final_raw_sequencer_mask=pass.raw_ah;
        for(const auto& write:pass.writes){
            ++result.bytes_compared;
            if(write.transparent){
                ++result.transparent_skipped;
                continue;
            }
            vga.write(write.address,write.mask,write.color);
            ++result.bytes_written;
        }
    }
    result.frame_source_bytes_advanced=static_cast<std::uint32_t>(
        source_pointer-frame_begin);
    original_state=s;
    return result;
}
} // namespace rloft
