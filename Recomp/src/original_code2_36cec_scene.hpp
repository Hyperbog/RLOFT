#pragma once
// Original retail CODE.2 36CEC: center-out RES3 x traversal, source-ray
// depth clipping and 36B14 -> 369D8 planar renderer integration.
// Byte authority CODE.2 SHA256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314.
// The guest must already have been filled by original 32954 and original
// 2F29C(archive=3); this never invents ray-visible actor positions.
#include "original_code2_36cec_columns.hpp"
#include "original_code2_369d8_raster.hpp"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_recomp {
struct Original36cecRasterMetrics {
    std::uint32_t source_columns{}, visible_runs{}, occluded_columns{};
    std::uint32_t plane_passes{}, indexed_writes{};
};
inline std::uint32_t original_36cec_dword(
    const std::vector<std::uint8_t>& g,std::uint64_t p) {
    if(p>=g.size() || g.size()-p<4)
        throw std::out_of_range("36CEC DWORD outside original guest");
    return std::uint32_t(g[p]) | std::uint32_t(g[p+1])<<8 |
           std::uint32_t(g[p+2])<<16 | std::uint32_t(g[p+3])<<24;
}
inline std::uint16_t original_36cec_table(
    const std::vector<std::uint8_t>& g,std::uint64_t p) {
    return original_369d8_word(g,p);
}
// 36B9C and 36BD8: original 16-bit width accumulator (wraps).
inline unsigned original_36cec_cutoff(
    const std::vector<std::uint8_t>& g,std::uint32_t table,
    unsigned threshold,bool from_bottom) {
    std::uint16_t sum=0;
    if(from_bottom) {
        int i=127;
        for(;i>=0;--i) {
            sum=static_cast<std::uint16_t>(
                sum+original_36cec_table(g,table+0x102u+2u*i));
            if(sum>threshold)break;
        }
        return static_cast<unsigned>(std::min(127,i+1));
    }
    unsigned i=0;
    for(;i<128;++i) {
        sum=static_cast<std::uint16_t>(
            sum+original_36cec_table(g,table+0x102u+2u*i));
        if(sum>threshold)break;
    }
    return i;
}
// The original branches at 36F29..37092 and 3713B..37298 test the
// FIRST and LAST rays in a run. If one end is obstructed, they trim
// toward the visible end. If both are obstructed they skip the run.
struct Original36cecClippedRun {int x; unsigned width;};
template<class ReadDepth>
Original36cecClippedRun original_36cec_clip_run(
    int x,unsigned width,unsigned ray_count,
    std::uint32_t sprite_depth,ReadDepth depth) {
    if(width==0 || ray_count==0)return {0,0};
    auto lo=std::max<std::int64_t>(0,x);
    auto hi=std::min<std::int64_t>(
        ray_count,static_cast<std::int64_t>(x)+width);
    if(hi<=lo)return {0,0};
    auto nearer=[&](std::int64_t ray) {
        return depth(static_cast<unsigned>(ray))<sprite_depth;
    };
    const bool first=nearer(lo),last=nearer(hi-1);
    if(!first && !last)return {0,0};
    if(first && !last)
        while(hi>lo && !nearer(hi-1))--hi;
    if(!first && last)
        while(lo<hi && !nearer(lo))++lo;
    return {static_cast<int>(lo),static_cast<unsigned>(hi-lo)};
}
inline Original36cecRasterMetrics raster_original_36cec_scene(
    std::vector<std::uint8_t>& guest,
    rloft::graphics::OriginalVgaHost& video,
    const Original36cecCall& call,
    std::uint32_t image, std::uint32_t image_length,
    std::uint32_t sintab_arena_first,
    std::uint32_t sintab_arena_end) {
    Original36cecRasterMetrics metrics{};
    if(call.projected_size<8)return metrics; // original 36D07
    if(image_length<14 || image<2 || image>=guest.size() ||
       static_cast<std::uint64_t>(image)+image_length>guest.size())
        throw std::out_of_range("36CEC decoded RES3 allocation bounds");
    const auto scale_max=original_36cec_dword(guest,0xC738u);
    const auto lookup_max=original_36cec_dword(guest,0xC730u);
    const auto lookup_base=original_36cec_dword(guest,0x258C4u);
    if(lookup_max>508u || std::uint64_t(lookup_base)+1018u>guest.size())
        throw std::out_of_range("36CEC original runtime lookup unavailable");
    std::vector<std::uint16_t> lookup(509);
    for(unsigned i=0;i<509;++i)
        lookup[i]=original_36cec_table(guest,lookup_base+2u*i);
    const auto plan=prepare_original_36cec(
        call,guest.data()+image,image_length,scale_max,
        lookup.data(),lookup.size(),lookup_max);
    if(!plan.drawable)return metrics;
    const auto code_pointer=original_36cec_dword(
        guest,0x18FE0u+4u*plan.scale_pointer_index);
    if(code_pointer<0x204u || code_pointer<sintab_arena_first+0x204u ||
       code_pointer>=sintab_arena_end)
        throw std::out_of_range("36CEC SINTAB scale code pointer");
    const std::uint32_t table=code_pointer-0x204u;
    if(table+0x204u>sintab_arena_end)
        throw std::out_of_range("36CEC short SINTAB scale table");
    const auto width=original_36cec_table(guest,image);
    const auto xmin=original_36cec_table(guest,image+2);
    const auto xmax=original_36cec_table(guest,image+4);
    const unsigned half=width/2u;
    const unsigned scaled_index=plan.scale_pointer_index*4u;
    const unsigned base_scale=plan.clamped_scale;
    const bool special=call.res3_id>=0x117u && call.res3_id<0x13Bu;
    const std::int64_t initial_phase=special
        ? (std::int64_t(base_scale)*2+(base_scale*2u>>3))
        : (base_scale>>2);
    std::int64_t phase=initial_phase+
        (static_cast<std::int64_t>(256)-scaled_index)/2+
        (scaled_index>>3);
    // All generated-opcode clipping is staged privately. The original
    // source patches 36C10/36C44 and later restores with 36C90/36CBC.
    auto staged=guest;
    auto staged_video=video;
    if(call.vertical_mode==0u) {
        phase+=scaled_index;
        unsigned cutoff=128;
        if(phase>256)
            cutoff=original_36cec_cutoff(
                staged,table,static_cast<unsigned>(phase-256),true);
        for(unsigned i=cutoff;i<128;++i) {
            const auto address=std::uint64_t(table)+
                               original_36cec_table(staged,table+2u*i);
            if(address<sintab_arena_first || address>=sintab_arena_end)
                throw std::out_of_range("36C10 SINTAB stop patch");
            staged[address]=0xC3u;
        }
    } else {
        if(phase<=0)return metrics; // original 36E2E
        const auto correction=
            ((std::uint64_t(scaled_index)*2u*call.vertical_mode)+64u)>>8;
        if(correction>static_cast<std::uint64_t>(INT32_MAX))
            throw std::out_of_range("36CEC vertical offset overflow");
        initial_phase; // original patch scratch state is not persistent
        if(phase<static_cast<std::int64_t>(correction)) {
            const auto cutoff=original_36cec_cutoff(
                staged,table,static_cast<unsigned>(correction-phase),false);
            for(unsigned i=0;i<cutoff;++i) {
                const auto offset=original_36cec_table(staged,table+2u*i);
                const auto next=original_36cec_table(staged,table+2u*i+2u);
                const auto at=std::uint64_t(table)+offset;
                if(at<sintab_arena_first || at+1u>=sintab_arena_end ||
                   next<offset+2u || next-offset-2u>127u)
                    throw std::out_of_range("36C44 SINTAB short jump");
                staged[at]=0xEBu;
                staged[at+1u]=static_cast<std::uint8_t>(next-offset-2u);
            }
        }
        phase=initial_phase-
            static_cast<std::int64_t>(correction); // 36E51 subtracts 10450
    }
    if(phase<INT32_MIN || phase>INT32_MAX)
        throw std::out_of_range("36CEC renderer phase out of range");
    const auto ray_count=original_36cec_dword(guest,0xC6D8u);
    if(ray_count!=256u)
        throw std::out_of_range("36CEC expects original 256 ray records");
    auto ray_depth=[&](unsigned ray)->std::uint32_t {
        return original_36cec_dword(guest,0x15A18u+28u*ray);
    };
    const auto mode=guest.at(0xC029u)!=0u;
    const auto row_base=original_36cec_dword(guest,0xC6E0u);
    const auto page=guest.at(0xEF4Du);
    int left=static_cast<std::int32_t>(call.screen_x);
    int right=left;
    for(const auto source_x:plan.source_x_order) {
        if(source_x<xmin || source_x>xmax)
            throw std::logic_error("36CEC generated source X out of bounds");
        const auto stretch=original_36cec_table(
            staged,table+0x102u+2u*source_x);
        if(stretch>0x100u)
            throw std::out_of_range("36CEC invalid horizontal stretch");
        ++metrics.source_columns;
        if(!stretch){++metrics.occluded_columns;continue;}
        const bool on_left=source_x<half;
        int start=on_left?(left-=stretch):right;
        if(!on_left)right+=stretch;
        const auto run=original_36cec_clip_run(
            start,stretch,ray_count,call.ray_depth,ray_depth);
        if(!run.width){++metrics.occluded_columns;continue;}
        const auto offset=original_36cec_table(
            staged,image+10u+2u*(source_x-xmin));
        const auto list=std::uint64_t(image)+offset;
        if(list<image+10u+2u*(xmax-xmin+1u) ||
           list>=std::uint64_t(image)+image_length)
            throw std::out_of_range("36CEC invalid original list");
        Original369d8Context ctx{};
        ctx.image_begin=image-2u;ctx.image_end=image+image_length;
        ctx.source_body=image;ctx.list_pointer=static_cast<std::uint32_t>(list);
        ctx.sintab_begin=table;ctx.sintab_end=sintab_arena_end;
        ctx.screen_x=run.x;ctx.run_width=
            static_cast<std::uint16_t>(run.width);
        ctx.packed_lookup=plan.packed_lookup;
        ctx.renderer_phase=static_cast<std::int32_t>(phase);
        ctx.row_base=row_base;ctx.video_page=page;
        ctx.individual_columns=mode;
        const auto raster=execute_original_369d8(staged,staged_video,ctx);
        metrics.indexed_writes+=raster.indexed_writes;
        metrics.plane_passes+=raster.plane_passes;
        ++metrics.visible_runs;
    }
    // Commit only the completed planar sprite; never carry SINTAB
    // self-patched stop instructions into the live original guest.
    video=std::move(staged_video);
    std::copy_n(staged.begin()+0xA0000u,0x10000u,guest.begin()+0xA0000u);
    return metrics;
}
} // namespace rloft_recomp
