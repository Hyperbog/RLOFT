#pragma once
// Original retail CODE.2 36CEC: center-out RES3 x traversal, source-ray
// depth clipping and 36B14 -> 369D8 planar renderer integration.
// Byte authority CODE.2 SHA256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314.
// The guest must already have been filled by original 32954 and original
// 2F29C(archive=3); this never invents ray-visible actor positions.
#include "../../original_code2_36cec_columns.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace rloft_recomp {
struct Original36cecRasterMetrics {
    std::uint32_t source_columns{}, visible_runs{}, occluded_columns{};
    std::uint32_t plane_passes{}, indexed_writes{}, off_aperture_cpu_writes{};
};
std::uint32_t original_36cec_dword(
    const std::vector<std::uint8_t>& g,std::uint64_t p) ;
std::uint16_t original_36cec_table(
    const std::vector<std::uint8_t>& g,std::uint64_t p) ;
// 36B9C and 36BD8: original 16-bit width accumulator (wraps).
unsigned original_36cec_cutoff(
    const std::vector<std::uint8_t>& g,std::uint32_t table,
    unsigned threshold,bool from_bottom) ;
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
Original36cecRasterMetrics raster_original_36cec_scene(
    std::vector<std::uint8_t>& guest,
    rloft::graphics::OriginalVgaHost& video,
    const Original36cecCall& call,
    std::uint32_t image, std::uint32_t image_length,
    std::uint32_t sintab_arena_first,
    std::uint32_t sintab_arena_end) ;
} // namespace rloft_recomp
