#include "rloft_native/native_type7_vga_dispatch.hpp"
#include "rloft_native/native_sha256.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
constexpr std::size_t kObject2=0x75F24U;
constexpr std::array<std::size_t,3> kMaskOffsets{
    0xC668U,0xC688U,0xC6A8U};
void require(bool ok,const char* why) {
    if(!ok)throw std::runtime_error(why);
}
unsigned popcount(std::uint8_t n) {
    unsigned count=0;
    for(;n;n>>=1U)count+=n&1U;
    return count;
}
std::uint32_t swap_ax(std::uint32_t ecx) noexcept {
    return (ecx&0xFFFF0000U) |
           ((ecx&0xFFU)<<8U) |
           ((ecx&0xFF00U)>>8U);
}
void append(NativeType7VgaDispatch& out,
            const NativeType7VgaMasks& masks,
            unsigned x,unsigned width,unsigned page,unsigned row,
            unsigned row_offset,std::uint32_t ecx) {
    const auto triplet=masks.for_run(x,static_cast<std::uint8_t>(width));
    // VGA physical base A0000 is omitted: indexed plane memory offsets
    // refer to [A0000,AFFFF] exactly, with each plane holding 65536 bytes.
    const auto physical=std::size_t(page)*32768U +
                        std::size_t(row)*80U+row_offset+x/4U;
    require(physical+2U<NativeIndexedVgaPlanes::kPlaneSize,
            "original VGA plane-pass address leaves A0000 aperture");
    for(unsigned pass=0;pass<3;++pass) {
        if(pass>0 && triplet[pass]==0)continue;
        out.calls.push_back({
            static_cast<std::uint16_t>(x),physical+pass,ecx,
            triplet[pass],static_cast<std::uint8_t>(pass)});
    }
}
} // namespace

NativeType7VgaMasks decode_native_type7_vga_masks(
    const std::vector<std::uint8_t>& code) {
    const auto state=kObject2+0xC029U;
    const auto page=kObject2+0xEF4DU;
    require(page<code.size() && state<code.size(),
            "original CODE.2 bound object 2 VGA globals missing");
    NativeType7VgaMasks out;
    require(code[state]<=1 && code[page]<=1,
            "original CODE.2 VGA flags/page outside Boolean domain");
    out.initial_individual_columns=code[state]!=0;
    out.initial_video_page=code[page];
    out.original_code2_sha256=native_sha256_hex(code);
    for(unsigned k=0;k<3;++k) {
        const auto begin=kObject2+kMaskOffsets[k];
        require(begin<=code.size() && code.size()-begin>=32,
                "original CODE.2 VGA plane mask table truncated");
        std::copy_n(code.begin()+static_cast<std::ptrdiff_t>(begin),32,
                    out.tables[k].begin());
    }
    // Exact source-domain masks: each of 28 legal (phase,width)
    // selectors covers the span once in strictly increasing byte passes.
    for(unsigned phase=0;phase<4;++phase) {
        for(unsigned width=1;width<=7;++width) {
            const auto slot=phase*8U+width;
            unsigned written=0;
            for(unsigned pass=0;pass<3;++pass) {
                const auto mask=out.tables[pass][slot];
                require((mask&0xF0U)==0,
                        "original CODE.2 sequencer mask uses non-VGA lane");
                written+=popcount(mask);
                std::uint8_t expected=0;
                for(unsigned col=0;col<width;++col) {
                    const auto shifted=phase+col;
                    if(shifted/4U==pass)
                        expected|=static_cast<std::uint8_t>(1U<<(shifted&3U));
                }
                require(mask==expected,
                        "CODE.2 VGA table differs from exact column-pass coverage");
            }
            require(written==width,"CODE.2 VGA mask coverage incomplete");
        }
    }
    return out;
}
std::array<std::uint8_t,3> NativeType7VgaMasks::for_run(
    std::uint32_t x,std::uint8_t width) const {
    require(width>0 && width<=7,
            "original CODE.2 grouped VGA mask run exceeds certified length");
    const auto slot=std::size_t(x&3U)*8U+width;
    return {tables[0][slot],tables[1][slot],tables[2][slot]};
}

NativeType7VgaDispatch plan_native_type7_vga_dispatch(
    const NativeType7VgaMasks& masks,
    unsigned x,std::uint8_t width,std::uint8_t page,
    unsigned row,unsigned row_offset,std::uint32_t ecx,
    bool individual) {
    require(page<=1 && row<400U && row_offset<80U &&
            x<320U && width>0 && width<=320U-x &&
            (individual || width<=7),
            "original TYPE7 VGA dispatch outside clipped/bounded viewport");
    NativeType7VgaDispatch out;
    out.original_video_page=page;
    out.individual_columns=individual;
    out.calls.reserve(std::size_t(width)*3U);
    if(!individual) {
        append(out,masks,x,width,page,row,row_offset,ecx);
        out.lookup_after_dispatch=swap_ax(ecx);
    } else {
        for(unsigned off=0;off<width;++off) {
            const auto column=x+off;
            append(out,masks,column,1,page,row,row_offset,
                   (column&1U)?swap_ax(ecx):ecx);
        }
        out.lookup_after_dispatch=ecx;
    }
    require(!out.calls.empty() && out.calls.size()<=765U,
            "original VGA dispatch produced too many plane calls");
    return out;
}
NativeType7VgaDraw draw_native_type7_vga_dispatch(
    const NativeSintabPrograms& tables,
    const NativeType7SceneShape& shape,
    unsigned scale,unsigned source_column,
    const std::vector<std::uint8_t>& body,
    std::int64_t base,
    const NativeType7VgaDispatch& dispatch,
    NativeIndexedVgaPlanes& target) {
    require(source_column<shape.columns.size(),
            "EH7 source column outside audited original shape");
    require(dispatch.calls.size()<=765U,
            "type7 VGA dispatch call count exceeds 255x3 limit");
    NativeIndexedVgaPlanes staged=target;
    NativeType7VgaDraw out;
    std::size_t first=0;
    while(first<dispatch.calls.size()) {
        std::size_t last=first+1;
        const auto column=dispatch.calls[first].viewport_column;
        require(column<320U,"type7 VGA dispatch viewport column invalid");
        while(last<dispatch.calls.size() &&
              dispatch.calls[last].viewport_column==column)++last;
        require(last-first<=3U &&
                dispatch.calls[first].pass_index==0,
                "type7 VGA dispatch has invalid plane-pass grouping");
        for(std::size_t i=first;i<last;++i) {
            const auto& call=dispatch.calls[i];
            require(call.pass_index==i-first &&
                    call.sequencer_mask<=15 &&
                    call.plane_byte_address<
                        NativeIndexedVgaPlanes::kPlaneSize,
                    "type7 VGA dispatch pass index/mask/address invalid");
        }
        const auto& descs=shape.columns[source_column].descriptors;
        for(unsigned d=0;d<descs.size();++d) {
            const auto plan=resolve_native_eh7_span(
                shape,tables,scale,source_column,d);
            ++out.descriptor_invocations;
            for(std::size_t i=first;i<last;++i) {
                const auto& call=dispatch.calls[i];
                const auto single=draw_native_eh7_span(
                    tables,plan,body,base,call.ecx_lookup,
                    call.plane_byte_address,call.sequencer_mask,staged);
                ++out.raster_call_invocations;
                out.indexed_source_reads+=single.source_reads;
                out.masked_vga_writes+=single.masked_destination_writes;
                out.individual_plane_writes+=single.individual_plane_writes;
            }
        }
        first=last;
    }
    target=std::move(staged);
    out.resulting_four_plane_sha256=target.planes_sha256();
    return out;
}
} // namespace rloft_native
