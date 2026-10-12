#include "rloft_native/native_type7_vga_dispatch.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft_native;
namespace {
template<class F> void rejects(F action) {
    bool rejected=false;
    try{action();}catch(const std::exception&){rejected=true;}
    assert(rejected);
}
std::vector<std::uint8_t> synthetic_code2() {
    constexpr std::size_t obj=0x75F24U;
    std::vector<std::uint8_t> code(obj+0xEF4DU+1U,0);
    constexpr std::size_t masks[]={0xC668U,0xC688U,0xC6A8U};
    for(unsigned phase=0;phase<4;++phase)
        for(unsigned n=1;n<=7;++n)
            for(unsigned c=0;c<n;++c) {
                const auto x=phase+c;
                const auto pass=x/4U;
                code[obj+masks[pass]+phase*8U+n] |=
                    static_cast<std::uint8_t>(1U<<(x&3U));
            }
    code[obj+0xC029U]=1;
    code[obj+0xEF4DU]=0;
    return code;
}
NativeSintabPrograms fixture_tables() {
    NativeSintabPrograms tables;
    NativeSintabProgram p;
    p.original_x86_bytes={
        0x8A,0x86,0,0,0,0,   // mov AL,source[ESI+0]
        0x88,0xC4,           // mov AH,AL
        0x29,0xC8,           // SUB EAX,ECX (full 32-bit borrow)
        0x88,0x87,0,0,0,0,   // MOV [EDI],AL
        0x88,0xA7,80,0,0,0, // MOV [EDI+80],AH
        0xC3};
    p.patch_offsets.fill(0x204U);
    p.patch_offsets[128]=0x204U+22U;
    p.scale_steps[128]=4;
    p.summed_scale_steps=4;
    tables.blocks.push_back(p);
    return tables;
}
NativeType7SceneShape fixture_shape() {
    NativeType7SceneShape shape;
    shape.source_width=64;
    shape.x_min=0;
    shape.x_max=0;
    shape.y_min=0;
    shape.y_max=127;
    shape.columns.resize(1);
    shape.columns[0].descriptors.push_back({256,0,0});
    return shape;
}
void run() {
    const auto original=synthetic_code2();
    auto masks=decode_native_type7_vga_masks(original);
    assert(masks.initial_individual_columns);
    assert(masks.initial_video_page==0);
    assert(masks.original_code2_sha256.size()==64);
    assert((masks.for_run(6,7)==std::array<std::uint8_t,3>{12,15,1}));
    assert((masks.for_run(5,1)==std::array<std::uint8_t,3>{2,0,0}));
    rejects([&]{static_cast<void>(masks.for_run(6,8));});
    rejects([&]{static_cast<void>(masks.for_run(6,0));});

    const auto ecx=0x0102U;
    const auto grouped=plan_native_type7_vga_dispatch(
        masks,6,7,0,0,0,ecx,false);
    assert(!grouped.individual_columns);
    assert(grouped.calls.size()==3);
    assert(grouped.lookup_after_dispatch==0x0201U);
    assert(grouped.calls[0].sequencer_mask==12);
    assert(grouped.calls[1].sequencer_mask==15);
    assert(grouped.calls[2].sequencer_mask==1);
    for(unsigned i=0;i<3;++i) {
        assert(grouped.calls[i].viewport_column==6);
        assert(grouped.calls[i].plane_byte_address==1+i);
        assert(grouped.calls[i].pass_index==i);
        assert(grouped.calls[i].ecx_lookup==ecx);
    }
    const auto individual=plan_native_type7_vga_dispatch(
        masks,6,7,0,0,0,ecx,true);
    assert(individual.individual_columns);
    assert(individual.calls.size()==7);
    assert(individual.lookup_after_dispatch==ecx);
    for(unsigned i=0;i<7;++i) {
        const auto x=6U+i;
        assert(individual.calls[i].viewport_column==x);
        assert(individual.calls[i].plane_byte_address==x/4U);
        assert(individual.calls[i].sequencer_mask==(1U<<(x&3U)));
        assert(individual.calls[i].pass_index==0);
        assert(individual.calls[i].ecx_lookup==
               (x&1U?0x0201U:0x0102U));
    }
    // 36B14 individual mode supports a contiguous run wider than the
    // grouped seven-column mask domain, without fabricated splitting.
    const auto wider=plan_native_type7_vga_dispatch(
        masks,5,19,1,10,7,ecx,true);
    assert(wider.calls.size()==19);
    assert(wider.calls.front().plane_byte_address==32768+10*80+7+1);
    assert(wider.calls.back().viewport_column==23);
    assert(wider.lookup_after_dispatch==ecx);
    rejects([&]{static_cast<void>(plan_native_type7_vga_dispatch(
        masks,5,8,0,0,0,ecx,false));});
    rejects([&]{static_cast<void>(plan_native_type7_vga_dispatch(
        masks,319,2,0,0,0,ecx,true));});
    rejects([&]{static_cast<void>(plan_native_type7_vga_dispatch(
        masks,5,1,2,0,0,ecx,true));});
    rejects([&]{static_cast<void>(plan_native_type7_vga_dispatch(
        masks,5,1,0,400,0,ecx,true));});

    const auto tables=fixture_tables();
    auto shape=fixture_shape();
    std::vector<std::uint8_t> body(128,0);
    body[0]=201;
    body[1]=202;
    NativeIndexedVgaPlanes grouped_video,individual_video;
    const auto g=draw_native_type7_vga_dispatch(
        tables,shape,0,0,body,0,grouped,grouped_video);
    const auto i=draw_native_type7_vga_dispatch(
        tables,shape,0,0,body,0,individual,individual_video);
    assert(g.descriptor_invocations==1);
    assert(g.raster_call_invocations==3);
    assert(g.indexed_source_reads==3);
    assert(g.masked_vga_writes==6);
    assert(g.individual_plane_writes==14);
    assert(i.descriptor_invocations==7);
    assert(i.raster_call_invocations==7);
    assert(i.indexed_source_reads==7);
    assert(i.masked_vga_writes==14);
    assert(i.individual_plane_writes==14);
    for(unsigned x=6;x<=12;++x) {
        assert(grouped_video.indexed_pixel(x,0)==199);
        assert(grouped_video.indexed_pixel(x,1)==200);
        assert(individual_video.indexed_pixel(x,0)==(x&1U?200:199));
        assert(individual_video.indexed_pixel(x,1)==(x&1U?199:200));
    }
    assert(grouped_video.planes_sha256()==g.resulting_four_plane_sha256);
    assert(individual_video.planes_sha256()==i.resulting_four_plane_sha256);
    assert(grouped_video.planes_sha256()!=individual_video.planes_sha256());

    // All descriptors must run for each original destination column.
    shape.columns[0].descriptors.push_back({256,1,0});
    NativeIndexedVgaPlanes twice_video;
    const auto twice=draw_native_type7_vga_dispatch(
        tables,shape,0,0,body,0,grouped,twice_video);
    assert(twice.descriptor_invocations==2);
    assert(twice.raster_call_invocations==6);
    assert(twice.indexed_source_reads==6);
    assert(twice.masked_vga_writes==12);
    for(unsigned x=6;x<=12;++x) {
        assert(twice_video.indexed_pixel(x,0)==200);
        assert(twice_video.indexed_pixel(x,1)==201);
    }
    // A *later* invalid descriptor cannot leak any writes by an
    // earlier successful descriptor or VGA mask pass.
    auto bad_shape=shape;
    bad_shape.columns[0].descriptors[1].signed_adjustment=-15;
    const auto before=twice_video.planes_sha256();
    rejects([&]{static_cast<void>(draw_native_type7_vga_dispatch(
        tables,bad_shape,0,0,body,0,grouped,twice_video));});
    assert(twice_video.planes_sha256()==before);
    auto bad_dispatch=grouped;
    bad_dispatch.calls[1].sequencer_mask=16;
    rejects([&]{static_cast<void>(draw_native_type7_vga_dispatch(
        tables,shape,0,0,body,0,bad_dispatch,twice_video));});
    assert(twice_video.planes_sha256()==before);

    auto corrupt=original;
    corrupt[0x75F24U+0xC668U+2*8U+7U]^=1;
    rejects([&]{static_cast<void>(decode_native_type7_vga_masks(corrupt));});
    corrupt=original;
    corrupt[0x75F24U+0xC029U]=2;
    rejects([&]{static_cast<void>(decode_native_type7_vga_masks(corrupt));});
    corrupt=original;
    corrupt.pop_back();
    rejects([&]{static_cast<void>(decode_native_type7_vga_masks(corrupt));});
}
}
int main(){run();}
