#include "rloft_native/native_type6_column_sampler.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
void require(bool valid,const char* reason) {
    if(!valid)throw std::runtime_error(reason);
}
struct Prepared {
    const NativeType1Raster* texture{};
    unsigned source_column{};
    unsigned x{};
    unsigned page{};
    unsigned top{};
    unsigned height{};
    unsigned source_rows_skip{};
    unsigned visible_height{};
    std::uint8_t shade{};
    std::uint32_t step{};
};
Prepared prepare(const NativeType6ProjectedColumn& r) {
    require(r.texture!=nullptr,"type-6 strip has null texture");
    const auto& t=*r.texture;
    require(t.frames==1U && (t.width==32U || t.width==64U) &&
            t.height==128U &&
            t.pixels.size()==std::size_t(t.width)*128U,
            "type-6 source must be one authentic 32/64 by 128 indexed texture");
    require(r.original_source_column<t.width && r.viewport_x<320U &&
            r.projected_height>=1U && r.projected_height<=256U &&
            r.active_400_line_page<=1U &&
            r.working_region_top_row<=144U &&
            r.host_pitch_pixels>=-96 && r.host_pitch_pixels<=96,
            "original type-6 column/height/VGA page out of bounds");
    const int original_y=int(r.working_region_top_row)+
                int((256U-r.projected_height)/2U)+r.host_pitch_pixels;
    const int first=std::max(original_y,int(r.working_region_top_row));
    const int last=std::min(original_y+int(r.projected_height),
                            int(r.working_region_top_row+256U));
    require(first>=0 && last<=400 && first<last,
            "type-6 pitched wall outside host presentation viewport");
    // CODE.2 338D3..338E5: unsigned source phase stride. For positive
    // height <=256 the final decoded source row cannot exceed 127.
    constexpr std::uint32_t source_domain=0x800000U;
    const auto step=source_domain/r.projected_height;
    require(step>0 && (std::uint64_t(r.projected_height-1U)*step>>16U)<128U,
            "original type-6 source step exceeds 128-texel column");
    return {&t,r.original_source_column,r.viewport_x,
            r.active_400_line_page,unsigned(first),r.projected_height,
            unsigned(first-original_y),unsigned(last-first),
            r.palette_index_subtraction,step};
}
}

NativeType6ColumnDraw draw_native_type6_columns(
    const std::vector<NativeType6ProjectedColumn>& calls,
    NativeIndexedVgaPlanes& target) {
    require(!calls.empty() && calls.size()<=1024U,
            "native type-6 texture draw call budget exceeded");
    // First reject any invalid texture or destination without altering
    // the caller's pixel state; then stage all valid writes as one batch.
    std::vector<Prepared> prepared;
    prepared.reserve(calls.size());
    for(const auto& call:calls)prepared.push_back(prepare(call));
    NativeIndexedVgaPlanes staged=target;
    NativeType6ColumnDraw result;
    result.columns=prepared.size();
    result.first_scanline=400;
    for(const auto& p:prepared) {
        const auto& t=*p.texture;
        const auto base=std::size_t(p.page)*32768U+
                        std::size_t(p.top)*80U+p.x/4U;
        const auto mask=static_cast<std::uint8_t>(1U<<(p.x&3U));
        result.first_scanline=result.first_scanline<p.top?
                              result.first_scanline:p.top;
        const auto bottom=std::size_t(p.top)+p.visible_height-1U;
        if(bottom>result.last_scanline)result.last_scanline=bottom;
        std::uint64_t phase=std::uint64_t(p.source_rows_skip)*p.step;
        for(unsigned pixel=0;pixel<p.visible_height;++pixel) {
            const auto sy=static_cast<unsigned>(phase>>16U);
            require(sy<128U,"type-6 fixed-point sample outside source column");
            const auto source=t.pixels[std::size_t(sy)*t.width+p.source_column];
            const auto output=static_cast<std::uint8_t>(
                (unsigned(source)-unsigned(p.shade))&255U);
            const auto offset=base+std::size_t(pixel)*80U;
            require(offset<NativeIndexedVgaPlanes::kPlaneSize,
                    "type-6 projected VGA address outside plane aperture");
            staged.masked_write(offset,mask,output);
            ++result.source_texel_reads;
            ++result.plane_writes;
            phase+=p.step;
        }
    }
    target=std::move(staged);
    result.resulting_four_plane_sha256=target.planes_sha256();
    return result;
}
} // namespace rloft_native
